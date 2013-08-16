#ifndef _PATHRUNNER_H_
#define _PATHRUNNER_H_

#include "Animation.h"
#include "Arduino.h"
#include "Color.h"

class Adafruit_NeoPixel;
class ColorInterpolator;
class Pong3Table;
class LedLayer;
class LedPath;

class PathRunner : public Animation {
public:
  PathRunner(LedPath* path, int max_length, ColorInterpolator* colors,
	     Pong3Table* table, LedLayer* layer=NULL);
  ~PathRunner();

  virtual void Step();

private:
  LedPath* path_;
  int start_idx_;
  const int max_length_;

  ColorInterpolator* colors_;
  int* prev_leds_;
  int prev_leds_start_;
  int direction_;
};

#endif //  _PATHRUNNER_H_
