//
// Created by Jose Perez on 4/1/25.
//

#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include <vector>
#include <unordered_map>
#include "Frame.h"
#include "RequestResults.h"
#include <fstream>
#include <iostream>
#include <regex>


struct Location {
  int record_id;
  int block_id;
};

class BufferPool {
private:
  std::vector<Frame> buffer;
  std::unordered_map<int, int > record_id_to_block_id;
  std::unordered_map<int, int> block_id_to_frame_id;

  int num_blocks = 0;
  int total_frames = 0;
  int used_frames = 0;
  int pinned_frames = 0;
  int last_evicted_frame_id = 0;


  Result get_record_from_disk(int block_id, int record_id);

  void write_block_to_disk(int frame_index);
  int find_victim_frame();
  int get_frame_for_block(int block_id);

  [[nodiscard]] bool all_frames_pinned() const {return total_frames == pinned_frames;}
  [[nodiscard]] bool all_frames_occupied() const {return total_frames == used_frames;}

  [[nodiscard]] bool is_block_id_valid(int block_id) const {return block_id >= 1 && block_id <= num_blocks;}

  Result read_block_from_disk(int block_id);
  [[nodiscard]] bool set_block_in_disk(int frame_id, int block_id);

  [[nodiscard]] int find_first_unpinned_frame_id() const;
  [[nodiscard]] int find_first_empty_frame_id() const;



public:
  explicit BufferPool(int num_frames): total_frames(num_frames), buffer(std::vector<Frame>(num_frames, Frame{})) {};
  void initialize();
  Result get_record(int record_id);
  Result set_record(int record_id, Record record);
  Result pin_block(int block_id);
  Result unpin_block(int block_id);


};

#endif //BUFFERPOOL_H