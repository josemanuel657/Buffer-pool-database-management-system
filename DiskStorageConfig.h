//
// Created by Jose Perez on 4/1/25.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <array>
#include <string>
#include <span>

constexpr std::string DISK_BLOCK_DIR = "./disk_blocks";

constexpr int RECORD_SIZE = 40;
constexpr int DISK_BLOCK_SIZE = 4000;
constexpr int RECORDS_PER_BLOCK = DISK_BLOCK_SIZE / RECORD_SIZE;

struct Record {
	std::array<char, RECORD_SIZE> data{};

	[[nodiscard]] std::string get_record_number_string() const { return {data.begin(), data.begin() + 8}; }
	[[nodiscard]] int get_record_number() const { return std::stoi(get_record_number_string()); }

	[[nodiscard]] std::string to_string() const {
		return {data.begin(), data.end()};
	}

	auto begin() { return data.begin(); }
	auto end() { return data.end(); }

	auto begin() const { return data.begin(); }
	auto end() const { return data.end(); }
};

struct Block {
	std::array<char, RECORDS_PER_BLOCK * RECORD_SIZE> data;

	std::span<Record> get_records() {
		return {reinterpret_cast<Record*>(data.data()), RECORDS_PER_BLOCK};
	}

	Record& get_record(size_t index) {
		if (index >= RECORDS_PER_BLOCK) throw std::out_of_range("Index out of range");
		return reinterpret_cast<Record*>(data.data())[index];
	}

	void set_record(size_t index, const Record& record) {
		if (index >= RECORDS_PER_BLOCK) throw std::out_of_range("Index out of range");
		reinterpret_cast<Record*>(data.data())[index] = record;
	}

	[[nodiscard]] std::string to_string() const {
		return {data.begin(), data.end()};
	}

	auto begin() { return data.begin(); }
	auto end() { return data.end(); }

	auto begin() const { return data.begin(); }
	auto end() const { return data.end(); }

	const Record& get_record(size_t index) const {
		if (index >= RECORDS_PER_BLOCK) throw std::out_of_range("Index out of range");
		return reinterpret_cast<const Record*>(data.data())[index];
	}
};


#endif //CONFIG_H
