#ifndef _ANIMATEPATHS_H_
#define _ANIMATEPATHS_H_

#include "Animation.h"

class ColorInterpolator;
class LedPath;

class AnimatePaths : public Animation {
public:
  // TODO: These should be classes, not enums.  They need different info.
  enum AnimationType {
    //AT_WINDOW,
    //AT_RUNNER,
    AT_CYCLE,
    AT_FULL_COLOR_CYCLE,
    NUM_ANIMATIONS
  };

  AnimatePaths(AnimationType anim_type, ColorInterpolator* colors,
               int speed,
	       Pong3Table* table, LedLayer* layer = NULL);
  virtual ~AnimatePaths();

  virtual void Step();

  void AddPath(LedPath* path);

  virtual int UpdateSetting(int new_setting);
  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

private:
  AnimationType anim_type_;
  ColorInterpolator* colors_;
  LedPath** paths_;
  unsigned char num_paths_;
  int step_;
  int speed_;
};

#endif // _ANIMATEPATHS_H_
