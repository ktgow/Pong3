#ifndef _CIRCULARBUFFER_H_
#define _CIRCULARBUFFER_H_

class CircularBuffer {
public:
  CircularBuffer(int size, int initial_value = 0);
  ~CircularBuffer();

  inline int GetElement(int element) const {
    return elements_[(start_ + element) % size_];
  }
  inline int GetSize() const { return size_; }

  void AddElement(int value);

private:
  int* elements_;
  int start_;
  int size_;
};

#endif // _CIRCULARBUFFER_H_
