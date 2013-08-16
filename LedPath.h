#ifndef _LEDPATH_H_
#define _LEDPATH_H_

#include "Arduino.h"

class LedPath {
public:
  LedPath();
  ~LedPath();

  void AddSegment(int start, int end);
  inline int GetLength() const { return length_; }
  int GetLed(int step) const;

  class Iterator {
  public:
    Iterator(const LedPath& path,
	     unsigned char seg_num,
	     short led, short direction)
      : path_(path),
        seg_num_(seg_num),
        led_(led),
        direction_(direction)
	{
	}

    Iterator& operator++() {
      if (led_ == path_.segments_[seg_num_].end) {
	++seg_num_;
	if (seg_num_ >= path_.num_segments_) {
	  led_ = 0;
	  return *this;
	}
	led_ = path_.segments_[seg_num_].start;
	direction_ = path_.Direction(path_.segments_[seg_num_]);
      } else {
	led_ += direction_;
      }
      return *this;
    }

    bool operator==(const Iterator& other) const {
      return (seg_num_ == other.seg_num_ &&
	      led_ == other.led_);
    }
    
    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    short operator*() const {
      return led_;
    }

  private:
    const LedPath& path_;
    short led_;
    short direction_;
    unsigned char seg_num_;
  };

  Iterator begin() const {
    return Iterator(*this, 0, segments_[0].start,
		    Direction(segments_[0]));
  }

  Iterator end() const {
    return Iterator(*this, num_segments_, 0, 0);
  }

private:
  friend class Iterator;
  struct Segment {
    short start, end;
  };

  Segment* segments_;
  unsigned char num_segments_;
  int length_;

  inline int Direction(const Segment& segment) const {
    return (segment.start < segment.end) ? 1 : -1;
  }
  inline int Length(const Segment& segment) const {
    return 1 + abs(segment.start - segment.end);
  }
};

#endif // _LEDPATH_H_
