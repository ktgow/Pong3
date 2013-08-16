#ifndef _COLORINTERPOLATOR_H_
#define _COLORINTERPOLATOR_H_

#include "Color.h"

class ColorInterpolator {
public:
  ColorInterpolator();
  virtual ~ColorInterpolator();

  virtual Color GetColor(int step, int num_steps) = 0;
};

#endif //  _COLORINTERPOLATOR_H_
