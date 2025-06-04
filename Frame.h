// Created by Jose Perez on 4/1/25.

#ifndef FRAME_H
#define FRAME_H

#include <array>
#include <optional>
#include <span>

#include "DiskStorageConfig.h"

class Frame {
public:
  bool set_block_id(int block_id);
  int get_block_id() const;

  bool set_record(int record_number, const Record& record);
  std::optional<Record> get_record(int record_number) const;

  void set_dirty(bool is_dirty);
  bool is_dirty() const;

  void set_pinned(bool is_pinned);
  bool is_pinned() const;

  bool is_empty() const;

  void set_content(Block&& block);

  const Block& get_content() const;

  std::span<Record> get_records();


private:
  int block_id = -1; // -1 indicates an uninitialized block
  Block content{};
  bool dirty = false;
  bool pinned = false;

  bool valid_record_number(int record_number) const;
};

#endif // FRAME_H