// Created by Jose Perez on 4/1/25.

#include "Frame.h"

bool Frame::set_block_id(const int block_id) {
  if (block_id < 0)
    return false;

  this->block_id = block_id;
  return true;
}

bool Frame::is_empty() const {
  return this->block_id == -1;
}

bool Frame::valid_record_number(int record_number) const {
  return record_number >= 1 || record_number <= (RECORDS_PER_BLOCK * this->get_block_id());
}


int Frame::get_block_id() const {
  return this->block_id;
}

bool Frame::set_record(const int record_number, const Record& record) {
  if (!valid_record_number(record_number))
    return false;

  const int record_index = (record_number - 1) % RECORDS_PER_BLOCK;

  this->content.set_record(record_index, record);

  this->dirty = true;
  return true;
}


void Frame::set_content(Block&& block) {
  this->content = std::move(block);
}

std::optional<Record> Frame::get_record(const int record_number) const {
  if (!valid_record_number(record_number))
    return std::nullopt;

  const int record_index = (record_number - 1) % RECORDS_PER_BLOCK;

  return this->content.get_record(record_index);
}

const Block& Frame::get_content() const {
  return this->content;
};

void Frame::set_dirty(const bool is_dirty) {
  this->dirty = is_dirty;
}

bool Frame::is_dirty() const {
  return this->dirty;
}

void Frame::set_pinned(const bool is_pinned) {
  this->pinned = is_pinned;
}

bool Frame::is_pinned() const {
  return this->pinned;
}

std::span<Record> Frame::get_records() {
  return this->content.get_records();
}