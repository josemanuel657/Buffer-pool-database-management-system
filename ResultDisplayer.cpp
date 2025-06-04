#include "ResultDisplayer.h"

void ResultDisplayer::display(const Result& result) {
    std::visit([](auto&& concrete_result) {
        ResultDisplayer::display_result(concrete_result);
        std::cout << std::endl;
    }, result);
}

void ResultDisplayer::display_result(const SuccessRecordFoundInBuffer& result) {
    std::cout << "Success: Record found in memory.\n";
    std::cout << "Block ID: " << result.block_id << "\n";
    std::cout << "Frame ID: " << result.frame_id << "\n";
    std::cout << "Record: " << result.record.to_string();
}

void ResultDisplayer::display_result(const SuccessRecordLoadedToEmptyBuffer& result) {
    std::cout << "Success: Record loaded from disk into an empty frame.\n";
    std::cout << "Block ID: " << result.block_id << "\n";
    std::cout << "Frame ID: " << result.frame_id << "\n";
    std::cout << "Record: " << result.record.to_string();
}

void ResultDisplayer::display_result(const SuccessRecordLoadedToReplacedBuffer& result) {
    std::cout << "Success: Record loaded from disk into a previously used frame. "
              << "The last block was evicted from memory.\n";
    std::cout << "Old Block ID: " << result.old_block_id << "\n";
    std::cout << "Old Frame Dirty: "
              << (result.old_frame_was_dirty ? "Yes (edited block was written back to disk)" : "No") << "\n";
    std::cout << "Block ID: " << result.block_id << "\n";
    std::cout << "Frame ID: " << result.frame_id << "\n";
    std::cout << "Record: " << result.record.to_string();
}

void ResultDisplayer::display_result(const SuccessRecordSetFromMemory& result) {
    std::cout << "Success: Record updated directly in memory. The frame was marked dirty.\n";
    std::cout << "Old Record: " << result.old_record.to_string() << "\n";
    std::cout << "Block ID: " << result.block_id << "\n";
    std::cout << "Frame ID: " << result.frame_id;
}

void ResultDisplayer::display_result(const SuccessRecordSetFromDisk& result) {
    std::cout << "Success: Record updated after loading the block from disk. The frame was marked dirty.\n";
    std::cout << "Old Record: " << result.old_record.to_string() << "\n";
    std::cout << "Block ID: " << result.block_id << "\n";
    std::cout << "Frame ID: " << result.frame_id;
}

void ResultDisplayer::display_result(const SuccessRecordSetFromDiskEvictedFrame& result) {
    std::cout << "Success: Record updated. The block was loaded from disk, requiring a frame eviction.\n";
    std::cout << "Evicted Block ID: " << result.evicted_block_id << "\n";
    std::cout << "Old Record: " << result.old_record.to_string() << "\n";
    std::cout << "Block ID: " << result.block_id << "\n";
    std::cout << "Frame ID: " << result.frame_id;
}

void ResultDisplayer::display_result(const ErrorAllBuffersOccupiedAndPinned& result) {
    std::cout << "Error: All buffers are occupied and pinned.";
}

void ResultDisplayer::display_result(const ErrorRecordNotFound& /*result*/) {
    std::cout << "Error: Record not found.";
}

void ResultDisplayer::display_result(const ErrorRecordNot40Bytes& result) {
    std::cout << "Error: Record is not 40 bytes long.";
}

void ResultDisplayer::display_result(const ErrorRecordIncorrectFormat& result) {
    std::cout << "Error: Record has an incorrect format.";
}

void ResultDisplayer::display_result(const ErrorInexistantBlockNumber& result) {
    std::cout << "Error: Nonexistent block number.";
    std::cout << result.message << "";
}

void ResultDisplayer::display_result(const ErrorCantUnpinUnloadedBuffer& /*result*/) {
    std::cout << "Error: Cannot unpin a block that is not in memory.";
}

void ResultDisplayer::display_result(const SuccessPinningDirectlyFromBuffer& result) {
    std::string action = result.pinning ? "Pinning" : "Unpinning";
    std::cout << "Success: " << action << " performed directly in the buffer.\n";
    std::cout << "Frame Number: " << result.frame_number << "\n";
    std::cout << "Block Number: " << result.block_number << "\n";
    std::string state = result.pinning ? "pinned" : "unpinned";
    std::cout << "Was the block already " << state << ": "
              << (result.pinning == result.was_pinned ? "Yes" : "No");
}

void ResultDisplayer::display_result(const SuccessPinningLoadedToEmptyBuffer& result) {
    std::string action = result.pinning ? "Pinning" : "Unpinning";
    std::cout << "Success: " << action
              << " completed after loading the block into an empty buffer.\n";
    std::cout << "Frame Number: " << result.frame_number;
}

void ResultDisplayer::display_result(const SuccessPinningLoadedToReplacedBuffer& result) {
    std::string action = result.pinning ? "Pinning" : "Unpinning";
    std::cout << "Success: " << action
              << " completed after loading the block into memory, evicting a frame.\n";
    if (result.was_dirty) {
        std::cout << "The evicted block was dirty and was written back to disk.\n";
    }
    std::cout << "Frame Number: " << result.frame_number << "\n";
    std::cout << "Evicted Block Number: " << result.old_block_number << "\n";
    std::cout << "Was the block dirty: " << (result.was_dirty ? "Yes" : "No");
}
