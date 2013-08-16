#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Arduino.h"

class ColorInterpolator;
class Pong3Table;
class LedLayer;

class Animation {
public:
  Animation(Pong3Table* table, LedLayer* layer = NULL);
  virtual ~Animation();

  virtual void Step() = 0;
  // Update to a new setting.  If the new setting is out of range, it should
  // wrap to 0.  Returns the number of the new setting.
  virtual int UpdateSetting(int new_setting) { return 0; }
  virtual void SetColors(ColorInterpolator*) {}

  inline LedLayer* GetLayer() { return layer_; }

protected:
  LedLayer* layer_;
  int setting_;
};

#endif // _ANIMATION_H_
