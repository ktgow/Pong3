#include "CircularBuffer.h"

#include "Arduino.h"

CircularBuffer::CircularBuffer(int size, int initial_value)
  : elements_(NULL),
    size_(0),
    start_(0) {
  elements_ = new int[size];
  if (elements_ == NULL) return;

  size_ = size;
  for (int i = 0; i < size_; ++i) {
    elements_[i] = initial_value;
  }
}

CircularBuffer::~CircularBuffer() {
  if (elements_ != NULL) {
    delete[] elements_;
  }
}

void CircularBuffer::AddElement(int value) {
  elements_[start_] = value;
  start_ = (start_ + 1) % size_;
}
