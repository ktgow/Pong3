#ifndef _RUNNER_H_
#define _RUNNER_H_

#include "Animation.h"
#include "Arduino.h"
#include "CircularBuffer.h"
#include "Color.h"

class ColorInterpolator;
class Pong3Table;
class LedLayer;

class Runner : public Animation {
public:
  Runner(int start_led, int max_length, ColorInterpolator* colors,
	 Pong3Table* table, LedLayer* layer=NULL);

  ~Runner();

  virtual void Step();

  virtual int UpdateSetting(int new_setting);
  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

  LedLayer* GetLayer() const { return layer_; }

private:
  int start_led_;
  const int max_length_;
  int speed_;

  ColorInterpolator* colors_;
  CircularBuffer prev_leds_;

  Pong3Table* table_;
};

#endif //  _RUNNER_H_
