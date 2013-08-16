#include "LedPath.h"

#include "Arduino.h"

LedPath::LedPath()
  : segments_(NULL),
    num_segments_(0),
    length_(0) {
}

LedPath::~LedPath() {
  free(segments_);
}

void LedPath::AddSegment(int start, int end) {
  Segment* old_alloc = segments_;
  segments_ = (Segment*)realloc(segments_, sizeof(Segment) * (num_segments_ + 1));
  if (segments_ == NULL) {
    segments_ = old_alloc;
    return;
  }

  segments_[num_segments_].start = start;
  segments_[num_segments_].end = end;
  length_ += Length(segments_[num_segments_]);
  ++num_segments_;
}

int LedPath::GetLed(int step) const {
  if (num_segments_ == 0) return 0;
  if (step < 0) {
    return segments_[0].start;
  } else if (step >= length_) {
    return segments_[num_segments_ - 1].end;
  }

  int offset = step;
  for (int seg_num = 0; seg_num < num_segments_; ++seg_num) {
    const Segment& segment = segments_[seg_num];
    int segment_length = Length(segment);
    if (offset >= segment_length) {
      offset -= segment_length;
    } else {
      return segment.start + offset * Direction(segment);
      break;
    }
  }

  return segments_[num_segments_ - 1].end;
}
