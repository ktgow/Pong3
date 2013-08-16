#ifndef _MULTIPLYCOLORINTERPOLATOR_H_
#define _MULTIPLYCOLORINTERPOLATOR_H_

#include "ColorInterpolator.h"

class MultiplyColorInterpolator : public ColorInterpolator {
public:
  MultiplyColorInterpolator(ColorInterpolator* interp1,
			    ColorInterpolator* interp2);

  virtual Color GetColor(int step, int num_steps);

private:
  ColorInterpolator* interp1_;
  ColorInterpolator* interp2_;
};

#endif // _MULTIPLYCOLORINTERPOLATOR_H_
