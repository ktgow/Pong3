#include "PulseColor.h"

#include "ColorInterpolator.h"
#include "Pong3Table.h"
#include "LedLayer.h"
#include "Util.h"

PulseColor::PulseColor(ColorInterpolator* colors, float window_size,
                       int speed, Pong3Table* table,
		       LedLayer* layer)
  : Animation(table, layer),
    colors_(colors),
    window_size_(window_size),
    speed_(speed),
    step_(0),
    pulse_direction_(1),
    table_(table) {
  num_steps_ = FindLongestSegmentLength(table_, Pong3Table::ST_RADIAL) +
    FindLongestSegmentLength(table_, Pong3Table::ST_BORDER) / 2;
}

void PulseColor::FillSection(int segment_num) {
  const Pong3Table::LedSegment* radial = table_->
    GetSegment(Pong3Table::ST_RADIAL, segment_num);
  const Pong3Table::LedSegment* border = table_->
    GetSegment(Pong3Table::ST_BORDER, segment_num);

  int radial_length = 1 + abs(radial->start - radial->end);
  int border_length = 1 + abs(border->start - border->end);
  int sweep_length = radial_length + border_length / 2;

  // Place the color window in the interpolator.
  int interpolator_range = sweep_length / window_size_;
  int window_range = interpolator_range - sweep_length;
  int window_start = window_range * step_ / (num_steps_ - 1);
  //int window_end = window_start + sweep_length - 1;

  // Sweep colors from the beginning of the window at the center to the
  // end of the window at the outside.
  int radial_dir = (radial->start < radial->end) ? 1 : -1;
  for (int led_step = 0; led_step < radial_length; ++led_step) {
    int led = radial->start + led_step * radial_dir;
    int color_pos = window_start + led_step;
    layer_->SetColor(led, colors_->GetColor(color_pos, interpolator_range));
  }
  
  // Along the borders, continue the sweep up to the center pixel, then back
  // down to the edge pixels.
  int border_dir = (border->start < border->end) ? 1 : -1;
  for (int led_step = 0; led_step < border_length; ++led_step) {
    int led = border->start + led_step * border_dir;
    int color_pos = window_start + radial_length +
      border_length / 2 - abs(border_length / 2 - led_step);
    layer_->SetColor(led, colors_->GetColor(color_pos, interpolator_range));
  }
}

void PulseColor::Step() {
  for (int segment = 0; segment < 6; ++segment) {
    FillSection(segment);
  }

  step_ += pulse_direction_ * speed_;
  if (step_ >= num_steps_ - 1) {
    step_ = num_steps_ - 1;
    pulse_direction_ = -1;
  } else if (step_ <= 0) {
    step_ = 0;
    pulse_direction_ = 1;
  }
}

