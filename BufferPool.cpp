#include "BufferPool.h"


Result BufferPool::get_record(const int record_id) {

	// record doesnt exist
	if (!record_id_to_block_id.contains(record_id))
		return ErrorRecordNotFound();

	// record is on disk
	const int block_id = record_id_to_block_id[record_id];
	if (!block_id_to_frame_id.contains(block_id))
		return get_record_from_disk(block_id, record_id);

	// record is in memory
	const int frame_id = block_id_to_frame_id[block_id];
	Frame& frame = this->buffer[frame_id - 1];
	const auto record = frame.get_record(record_id);
	if (!record.has_value()) return ErrorRecordNotFound();
	return SuccessRecordFoundInBuffer(block_id, frame_id, record.value());
}

Result BufferPool::get_record_from_disk(int block_id, int record_id) {
	// no frames avaialble
	if (this->all_frames_pinned()) return ErrorAllBuffersOccupiedAndPinned{};

	// read the block from disk
	const std::string filename = DISK_BLOCK_DIR + "/F" + std::to_string(block_id) + ".txt"; //open th file
	std::ifstream block_file(filename, std::ios::binary);
	if (!block_file) {return ErrorRecordNotFound{"File not found: " + filename};} // couldn't open it

	// read from the file
	Block block = {};
	block_file.read(block.begin(),DISK_BLOCK_SIZE);
	if (block_file.gcount() == 0) {return ErrorRecordNotFound{"File is empty: " + filename};} //unsuccessfull read

	// We haven't used all frames yet
	if (!this->all_frames_occupied()) {
		int frame_id = find_first_empty_frame_id();
		if (frame_id == -1) return ErrorRecordNotFound{"No available frame found."};
		used_frames ++;

		Frame& frame = this->buffer[frame_id - 1];
		frame.set_block_id(block_id);
		frame.set_content(std::move(block));

		this->block_id_to_frame_id[block_id] = frame_id;

		auto record = record_id != -1? frame.get_record(record_id) : Record();

		return SuccessRecordLoadedToEmptyBuffer(block_id, frame_id, record.value_or(Record()));
	}

	// We have used all frames, we have to discard a frame
	int frame_id = find_first_unpinned_frame_id();
	Frame& frame = this->buffer[frame_id - 1];
	int last_block_id = frame.get_block_id();
	bool frame_was_dirty = frame.is_dirty();

	// the frame we discard was dirty, so put it in the database
	if (frame_was_dirty) {
		if (!this->set_block_in_disk(frame_id, last_block_id)) // SET BLOCK IN DISK?
			return ErrorRecordNotFound{"Failed to set block in disk"};
		this->block_id_to_frame_id.erase(last_block_id);
		this->block_id_to_frame_id[block_id] = frame_id;
		this->last_evicted_frame_id = frame_id;

		frame.set_block_id(block_id);
		frame.set_content(std::move(block));

		auto record = record_id != -1? frame.get_record(record_id).value_or(Record()) : Record();
		return SuccessRecordLoadedToReplacedBuffer(last_block_id,frame_was_dirty,block_id,frame_id,record);
	}

	// the frame we discardted was clean
	frame.set_block_id(block_id);
	frame.set_content(std::move(block));

	this->block_id_to_frame_id.erase(last_block_id);
	this->block_id_to_frame_id[block_id] = frame_id;
	this->last_evicted_frame_id = frame_id;

	auto record = record_id != -1? frame.get_record(record_id).value_or(Record()) : Record();
	return SuccessRecordLoadedToReplacedBuffer(last_block_id,frame_was_dirty,block_id,frame_id,record);
}

bool BufferPool::set_block_in_disk(int frame_id, int block_id) {
	// open the file using the block id
	const std::string filename = DISK_BLOCK_DIR + "/F" + std::to_string(block_id) + ".txt";
	std::ofstream block_file(filename, std::ios::binary);
	if (!block_file) return false; // unsuccessfully file read

	// write to the block
	Frame& frame = this->buffer[frame_id - 1];
	const Block& block = frame.get_content();
	block_file.write(block.begin(), DISK_BLOCK_SIZE);

	// now the frame is clean
	frame.set_dirty(false);

	return true; // success
}


int BufferPool::find_first_empty_frame_id() const {
	for (int i = 0; i < this->buffer.size(); ++i) {
		const Frame& frame = this->buffer[i];
		if (frame.is_empty()) {
			return i + 1;
		}
	}
	return -1;
}


int BufferPool::find_first_unpinned_frame_id() const {
	// no frames avaiable
	if (this->all_frames_pinned()) return -1;

	// find the first unpinned frame, take as a reference the last frame that we evicted
	for (int count = 0; count < this->total_frames; ++count) {
		const int frame_id = (last_evicted_frame_id + count) % this->total_frames + 1;
		const Frame& frame = this->buffer[frame_id - 1];
		if (!frame.is_pinned()) {
			return frame_id;
		}
	}

	return -1;
}

Result BufferPool::set_record(const int record_id, const Record record) {
	// No such record exists
	if (!record_id_to_block_id.contains(record_id)) return ErrorRecordNotFound{"Record does not exist"};
	int block_id = record_id_to_block_id[record_id];


	bool record_in_memory = block_id_to_frame_id.contains(block_id);
	bool evicted_frame = false;
	int evicted_block_id = -1;

	// Record is on disk, so get it
	if (!record_in_memory) {
		Result result = get_record_from_disk(block_id, record_id);
		if (std::holds_alternative<SuccessRecordLoadedToReplacedBuffer>(result)) {
			evicted_frame = true;
			evicted_block_id = std::get<SuccessRecordLoadedToReplacedBuffer>(result).old_block_id;
		}
		if (std::holds_alternative<ErrorAllBuffersOccupiedAndPinned>(result)) {
			return ErrorAllBuffersOccupiedAndPinned{};
		}
	}

	// set the record
	const int frame_id = block_id_to_frame_id[block_id];
	Frame& frame = this->buffer[frame_id - 1];
	Record old_record = frame.get_record(record_id).value();
	if (!frame.set_record(record_id, record)) {return ErrorRecordNotFound{"Failed to set record in frame"};}

	// the block was alredy in memory
	if (record_in_memory) return SuccessRecordSetFromMemory{old_record, block_id, frame_id};
	// the block was in the disk, and all the frames were full
	if (evicted_frame) return SuccessRecordSetFromDiskEvictedFrame{old_record, block_id, frame_id, evicted_block_id};
	// the block was in the disk, but there weere still empty frames that we could use
	return SuccessRecordSetFromDisk{old_record, block_id, frame_id};
}


Result BufferPool::pin_block(int block_id) {
	// invalid block id
	if (!this->is_block_id_valid(block_id)) {
		return ErrorInexistantBlockNumber("Blocks available: [1, ... " + std::to_string(this->num_blocks) + "]");
	}

	// Pinned in the buffer (the block was in memory)
	if (block_id_to_frame_id.contains(block_id)) {
		int frame_id = block_id_to_frame_id[block_id];
		Frame& frame = buffer[frame_id - 1];
		bool was_pinned = frame.is_pinned();
		frame.set_pinned(true);
		if (!was_pinned) this->pinned_frames++;
		return SuccessPinningDirectlyFromBuffer(true, frame_id, block_id, was_pinned);
	}

	// Block is on disk so get it
	Result result = get_record_from_disk(block_id, -1);

	if (std::holds_alternative<SuccessRecordLoadedToReplacedBuffer>(result)) {
		auto loading_operation = std::get<SuccessRecordLoadedToReplacedBuffer>(result);
		int frame_id = loading_operation.frame_id;
		Frame& frame = this->buffer[frame_id - 1];
		frame.set_pinned(true);
		this->pinned_frames++;
		return SuccessPinningLoadedToReplacedBuffer{true, loading_operation.old_block_id, frame_id, loading_operation.old_frame_was_dirty };
	}
	else if (std::holds_alternative<SuccessRecordLoadedToEmptyBuffer>(result)) {
		auto loading_operation = std::get<SuccessRecordLoadedToEmptyBuffer>(result);
		int frame_id = loading_operation.frame_id;
		Frame& frame = this->buffer[frame_id - 1];
		frame.set_pinned(true);
		this->pinned_frames++;
		return SuccessPinningLoadedToEmptyBuffer{true, frame_id};
	}
	else if (std::holds_alternative<ErrorAllBuffersOccupiedAndPinned>(result)) {
		return ErrorAllBuffersOccupiedAndPinned{};
	}
	else
		return ErrorRecordNotFound{"Failed to get record from disk"};
}

Result BufferPool::unpin_block(int block_id) {
	// invalid block id
	if (!this->is_block_id_valid(block_id)) {return ErrorInexistantBlockNumber("Blocks available: [1, ... " + std::to_string(this->num_blocks) + "]");}

	// Pinned in the buffer (the block was in memory)
	if (block_id_to_frame_id.contains(block_id)) {
		int frame_id = block_id_to_frame_id[block_id];
		Frame& frame = buffer[frame_id - 1];
		bool was_pinned = frame.is_pinned();
		frame.set_pinned(false);
		if (was_pinned) this->pinned_frames--;
		return SuccessPinningDirectlyFromBuffer(false,frame_id, block_id, was_pinned);
	}

	return ErrorCantUnpinUnloadedBuffer{};
}

// read the disk_blocks
void BufferPool::initialize() {
	for (const auto& file : std::filesystem::directory_iterator(DISK_BLOCK_DIR)) {
		if (file.is_regular_file() && file.path().extension() == ".txt") {
			std::ifstream disk(file.path());
			if (!disk) {
				std::cerr << "Could not open file: " << file.path() << std::endl;
				continue;
			}

			this->num_blocks++;

			std::string content((std::istreambuf_iterator<char>(disk)), std::istreambuf_iterator<char>());
			disk.close();

			// Extract record number
			std::regex record_pattern(R"(F\d{1,2}-Rec(\d{3}))");
			std::sregex_iterator iter(content.begin(), content.end(), record_pattern);
			std::sregex_iterator end;

			// Extract records and populate the hashmap
			int block_id = std::stoi(file.path().stem().string().substr(1)); // Extract block ID from file

			while (iter != end) {
				int record_number = std::stoi((*iter)[1].str()); // Extract record number

				// Store the mapping of record number to block Id
				record_id_to_block_id[record_number] = block_id;

				std::cout << "[record_number, block_id]: [" << record_number << ", " << block_id << "]" << std::endl;

				++iter;
			}
		}
	}
}







