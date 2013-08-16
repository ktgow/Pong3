#ifndef _PULSECOLOR_H_
#define _PULSECOLOR_H_

#include "Arduino.h"
#include "Animation.h"

class ColorInterpolator;
class Pong3Table;
class LedLayer;

// Pulse a color interpolator from the center out to the middle of each
// border.  The colors displayed are determined by a sliding window (based
// on window_size, from 0.0 to 1.0).  The window slides up to 1.0 then back
// down to 0.0.
class PulseColor : public Animation {
public:
  PulseColor(ColorInterpolator* colors, float window_size, int speed,
	     Pong3Table* table,
	     LedLayer* layer=NULL);

  virtual void Step();
  
  inline LedLayer* GetLayer() { return layer_; }

private:
  ColorInterpolator* colors_;
  float window_size_;
  int speed_;
  int step_;
  int num_steps_;
  int pulse_direction_;
  Pong3Table* table_;

  void FillSection(int segment_num);
};

#endif // _PULSECOLOR_H_
