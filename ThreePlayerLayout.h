#ifndef _THREEPLAYERLAYOUT_H_
#define _THREEPLAYERLAYOUT_H_

#include "Animation.h"

class ColorInterpolator;
class Pong3Table;

class ThreePlayerLayout : public Animation {
public:
  enum AnimationType {
    AT_OUTSIDE_TO_INSIDE,
    AT_INSIDE_TO_OUTSIDE
  };

  ThreePlayerLayout(ColorInterpolator* colors, int speed,
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
};

#endif // _THREEPLAYERLAYOUT_H_
