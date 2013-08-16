#include "SolidColorInterpolator.h"

SolidColorInterpolator::SolidColorInterpolator(const Color& color)
  : color_(color) {
}

Color SolidColorInterpolator::GetColor(int step, int num_steps) {
  return color_;
}
