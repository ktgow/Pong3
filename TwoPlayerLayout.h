#ifndef _TWOPLAYERLAYOUT_H_
#define _TWOPLAYERLAYOUT_H_

#include "Animation.h"

class ColorInterpolator;
class Pong3Table;

class TwoPlayerLayout : public Animation {
public:
  enum AnimationType {
    AT_SWEEP_ACROSS,
    AT_OUTSIDE_TO_INSIDE,
    AT_INSIDE_TO_OUTSIDE,
    AT_NUM_ANIMATIONS
  };

  TwoPlayerLayout(ColorInterpolator* colors, int speed,
		  int start_radial, AnimationType anim_type,
		  Pong3Table* table, LedLayer* layer = NULL);

  virtual void Step();

  virtual int UpdateSetting(int new_setting);
  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

private:
  ColorInterpolator* colors_;
  int step_, speed_;
  int start_radial_;
  AnimationType anim_type_;
  Pong3Table* table_;
  int setting_;
};

#endif // _TWOPLAYERLAYOUT_H_
