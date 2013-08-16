#ifndef _RANDOMLIGHTS_H_
#define _RANDOMLIGHTS_H_

#include "Animation.h"

class ColorInterpolator;

class RandomLights : public Animation {
public:
  RandomLights(ColorInterpolator* colors, Pong3Table* table, LedLayer* layer = NULL);
  virtual ~RandomLights();

  virtual void Step();

  // Update to a new setting.  If the new setting is out of range, it should
  // wrap to 0.  Returns the number of the new setting.
  virtual int UpdateSetting(int new_setting) { return 0; }
  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

private:
  ColorInterpolator* colors_;
  Pong3Table* table_;
};

#endif // _RANDOMLIGHTS_H_
