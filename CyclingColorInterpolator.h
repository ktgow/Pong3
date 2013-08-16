#ifndef _CYCLINGCOLORINTERPOLATOR_H_
#define _CYCLINGCOLORINTERPOLATOR_H_

#include "ColorInterpolator.h"

class CyclingColorInterpolator : public ColorInterpolator {
public:
  CyclingColorInterpolator(ColorInterpolator* colors);

  virtual Color GetColor(int step, int num_steps);

private:
  ColorInterpolator* colors_;
};

#endif // _CYCLINGCOLORINTERPOLATOR_H_
