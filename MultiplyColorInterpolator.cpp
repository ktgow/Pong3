#include "MultiplyColorInterpolator.h"

MultiplyColorInterpolator::MultiplyColorInterpolator(
    ColorInterpolator* interp1,
    ColorInterpolator* interp2) 
  :  interp1_(interp1), interp2_(interp2) {
}

Color MultiplyColorInterpolator::GetColor(int step, int num_steps) {
  Color color1 = interp1_->GetColor(step, num_steps);
  Color color2 = interp2_->GetColor(step, num_steps);

  return Color((int)color1.red * (int)color2.red / 255,
	       (int)color1.green * (int)color2.green / 255,
	       (int)color1.blue * (int)color2.blue / 255,
	       (int)color1.alpha * (int)color2.alpha / 255);
}
