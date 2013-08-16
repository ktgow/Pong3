#include "CyclingColorInterpolator.h"

CyclingColorInterpolator::CyclingColorInterpolator(ColorInterpolator* colors)
  : colors_(colors) {
}

Color CyclingColorInterpolator::GetColor(int step, int num_steps) {
  step %= num_steps;
  if (step < 0) step += num_steps;

  return colors_->GetColor(step, num_steps);
}
