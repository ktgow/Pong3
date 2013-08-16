#ifndef _COLORSET_H_
#define _COLORSET_H_

#include "Color.h"

class ColorInterpolator;

class ColorSet {
public:
  ColorSet();
  ~ColorSet();

  struct ColorKeyframeInfo {
    Color color;
    float dist;
  };

  void AddColors(const ColorKeyframeInfo* colors,
		 const int colors_size);
  void AddColors(ColorInterpolator* colors);

  int GetNumColors() const { return num_colors_; }
  ColorInterpolator* GetColor(int color);

  void DeleteAll();

private:
  ColorInterpolator** colors_;
  int num_colors_;
};

#endif // _COLORSET_H_
