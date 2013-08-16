#ifndef _BORDERCYCLE_H_
#define _BORDERCYCLE_H_

#include "Arduino.h"
#include "Animation.h"

class ColorInterpolator;
class Pong3Table;
class LedLayer;

class BorderCycle : public Animation {
public:
 BorderCycle(ColorInterpolator* border_colors,
	     ColorInterpolator* center_colors,
	     int speed,
	     Pong3Table* table,
	     LedLayer* layer=NULL);

  virtual void Step();

  virtual int UpdateSetting(int new_setting);
  virtual void SetColors(ColorInterpolator* colors) { border_colors_ = colors; }

private:
  ColorInterpolator* border_colors_;
  ColorInterpolator* center_colors_;
  int speed_;
  int step_;
  int num_steps_;
  Pong3Table* table_;
};

#endif // _BORDERCYCLE_H_
