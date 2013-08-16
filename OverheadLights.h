#ifndef _OVERHEADLIGHTS_H_
#define _OVERHEADLIGHTS_H_

#include "Animation.h"
#include "Color.h"

class OverheadLights : public Animation {
public:
  OverheadLights(ColorInterpolator* colors, Pong3Table* table,
		 LedLayer* layer = NULL);
  virtual ~OverheadLights();

  virtual void Step();
  virtual int UpdateSetting(int new_setting);
  virtual void SetColors(ColorInterpolator* colors);

  void LockColor(const Color& color);

private:
  enum {
    RED_PIN = 9,
    GREEN_PIN = 10,
    BLUE_PIN = 3,

    NUM_STEPS = 64,
  };

  ColorInterpolator* colors_;
  int step_;

  bool color_is_locked_;
  Color locked_color_;

  void SetColor(const Color& color);
};

#endif // _OVERHEADLIGHTS_H_
