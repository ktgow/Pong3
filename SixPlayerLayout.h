#ifndef _SIXPLAYERLAYOUT_H_
#define _SIXPLAYERLAYOUT_H_

#include "Animation.h"

class ColorInterpolator;
class Pong3Table;

class SixPlayerLayout : public Animation {
public:
  enum AnimationType {
    AT_OUTSIDE_TO_INSIDE,
    AT_INSIDE_TO_OUTSIDE
  };

  SixPlayerLayout(ColorInterpolator* colors, int speed,
		  bool spiral, AnimationType anim_type,
		  Pong3Table* table, LedLayer* layer = NULL);

  virtual void Step();

  virtual int UpdateSetting(int new_setting);
  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

private:
  ColorInterpolator* colors_;
  int step_, speed_;
  bool spiral_;
  AnimationType anim_type_;
  Pong3Table* table_;
};

#endif // _SIXPLAYERLAYOUT_H_
