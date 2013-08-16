#include "Util.h"

#include "Pong3Table.h"

int FindLongestSegmentLength(Pong3Table* table, int segment_type) {
  int longest = 0;
  for (int segment_num = 0; segment_num < 6; ++segment_num) {
    const Pong3Table::LedSegment* segment = table->
      GetSegment((Pong3Table::SegmentType)segment_type, segment_num);
    int distance = 1 + abs(segment->start - segment->end);
    longest = max(longest, distance);
  }
  return longest;
}

int availableMemory() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

ComputeCycleLength::ComputeCycleLength()
  : num_lengths_(0) {
}

void ComputeCycleLength::AddLength(int length) {
  for (int i = 0; i < num_lengths_; ++i) {
    if (lengths_[i] == length) {
      return;
    }
  }

  if (num_lengths_ < MAX_LENGTHS) {
    lengths_[num_lengths_++] = length;
  }
}

int ComputeCycleLength::GetCycleLength() const {
  if (num_lengths_ == 0) return 0;

  int max_int = 0x7f;
  for (int i = 0; i < sizeof(int) - 1; ++i) {
    max_int <<= 8;
    max_int |= 0xff;
  }

  int cycle_length = 1;
  for (int i = 0; i < num_lengths_; ++i) {
    if (max_int / lengths_[i] < cycle_length) {
      // Next multiply would cause an overflow.
      return max_int;
    }
    cycle_length *= lengths_[i];
  }

  return cycle_length;
}
