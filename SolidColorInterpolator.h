#ifndef _SOLIDCOLORINTERPOLATOR_H_
#define _SOLIDCOLORINTERPOLATOR_H_

#include "Color.h"
#include "ColorInterpolator.h"

class SolidColorInterpolator : public ColorInterpolator {
public:
  SolidColorInterpolator(const Color& color);

  virtual Color GetColor(int step, int num_steps);

private:
  Color color_;
};

#endif // _SOLIDCOLORINTERPOLATOR_H_
