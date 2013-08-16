#ifndef _JELLYFISH_H_
#define _JELLYFISH_H_

#include "Animation.h"
#include "Arduino.h"

class Adafruit_NeoPixel;
class ColorInterpolator;
class Pong3Table;
class LedLayer;

class JellyFish : public Animation {
public:
  JellyFish(int max_length, ColorInterpolator* colors, int speed,
	    Pong3Table* table, LedLayer* layer=NULL);

  virtual void Step();

  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

  LedLayer* GetLayer() const { return layer_; }

private:
  const int max_length_;

  ColorInterpolator* colors_;

  Pong3Table* table_;

  enum Stage {
    MOVE_OUT,
    MOVE_ACROSS,
    MOVE_IN
  };
  Stage stage_;
  int step_;
  int speed_;

  void SetSegmentColors(int segment_type, int segment_num,
			int start_fraction, int max_segment_len,
                        int direction_multiplier);
};

#endif // _JELLYFISH_H_
