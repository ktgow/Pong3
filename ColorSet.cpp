#include "ColorSet.h"

#include "Arduino.h"
#include "KeyFrameColorInterpolator.h"

ColorSet::ColorSet()
  : colors_(NULL),
    num_colors_(0) {
}

ColorSet::~ColorSet() {
  DeleteAll();
}

void ColorSet::AddColors(const ColorKeyframeInfo* colors,
			 const int colors_size) {
  KeyFrameColorInterpolator* color = NULL;
  for (int i = 0; i < colors_size; ++i) {
    if (color == NULL) {
      color = new KeyFrameColorInterpolator;
      if (color == NULL) {
	return;
      }

      AddColors(color);
    }

    color->AddKeyFrame(
        KeyFrameColorInterpolator::KeyFrame(
            colors[i].color,
	    colors[i].dist * KeyFrameColorInterpolator::KeyFrame::DIST_MAX));

    if (colors[i].dist >= 1.0) {
      color = NULL;
    }
  }
}

void ColorSet::AddColors(ColorInterpolator* colors) {
  void* new_colors = realloc(colors_, sizeof(ColorInterpolator*) * (num_colors_ + 1));
  if (new_colors == NULL) {
    return;
  }
  colors_ = (ColorInterpolator**)new_colors;
  ++num_colors_;

  colors_[num_colors_ - 1] = colors;
}

ColorInterpolator* ColorSet::GetColor(int color) {
  return colors_[min(num_colors_ - 1, color)];
}

void ColorSet::DeleteAll() {
  if (colors_ != NULL) {
    for (int i = 0; i < num_colors_; ++i) {
      delete colors_[i];
    }
    free(colors_);

    colors_ = NULL;
    num_colors_ = 0;
  }
}
