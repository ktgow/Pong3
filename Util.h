#ifndef _UTIL_H_
#define _UTIL_H_

#include "Arduino.h"
#include "Color.h"

class Pong3Table;

inline int interp(int start, int end, int step, int num_steps) {
  return start * (num_steps - 1 - step) / (num_steps - 1) +
    end * step / (num_steps - 1);
}

inline Color interp(Color start, Color end, int step, int num_steps) {
  Color result(interp(start.red, end.red, step, num_steps),
	       interp(start.green, end.green, step, num_steps),
	       interp(start.blue, end.blue, step, num_steps),
               interp(start.alpha, end.alpha, step, num_steps));
    /*
    Serial.print("Color: ");
    Serial.print(result.red);
    Serial.print(", ");
    Serial.print(result.green);
    Serial.print(", ");
    Serial.print(result.blue);
    Serial.print(", ");
    Serial.println(result.alpha);
*/
  return result;
}

int FindLongestSegmentLength(Pong3Table* table, int segment_type);
int availableMemory();

class ComputeCycleLength {
public:
  ComputeCycleLength();

  void AddLength(int length);
  int GetCycleLength() const;

private:
  enum { MAX_LENGTHS = 16 };
  int lengths_[MAX_LENGTHS];
  int num_lengths_;
};

#endif // _UTIL_H_
