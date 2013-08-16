#include "ThreePlayerLayout.h"

#include "ColorInterpolator.h"
#include "Pong3Table.h"
#include "LedLayer.h"

ThreePlayerLayout::ThreePlayerLayout(ColorInterpolator* colors,
				     int speed,
				     int start_radial, AnimationType anim_type,
				     Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    colors_(colors),
    speed_(speed),
    step_(0),
    start_radial_(start_radial),
    anim_type_(anim_type),
    table_(table) {
}

void ThreePlayerLayout::Step() {
  long length_product = 1;
  const Pong3Table::LedSegment* segments[3];
  int last_length = -1;
  for (int seg_num = 0; seg_num < 3; ++seg_num) {
    segments[seg_num] = table_->GetSegment(Pong3Table::ST_RADIAL,
					   (start_radial_ + 2 * seg_num) % 6);
    const int length = 1 + abs(segments[seg_num]->start - segments[seg_num]->end);
    if ((last_length == -1) || (length != last_length)) {
      length_product *= length;
    }
    last_length = length;
  }

  step_ = (step_ + speed_) % length_product;

  int inner_outer_dir = -1;
  if (anim_type_ == AT_OUTSIDE_TO_INSIDE) inner_outer_dir = 1;

  for (int seg_num = 0; seg_num < 3; ++seg_num) {
    const Pong3Table::LedSegment* segment = segments[seg_num];
    const int length = 1 + abs(segment->start - segment->end);
    const int dir = (segment->start < segment->end) ? 1 : -1;

    for (int i = 0; i < length; ++i) {
      int led = segment->start + i * dir;
      layer_->SetColor(led,
		       colors_->GetColor((inner_outer_dir * step_ + i) % length,
					 length));
    }
  }
}

int ThreePlayerLayout::UpdateSetting(int new_setting) {
  const int num_settings = 4;

  // Set current radials to black.
  for (int seg_num = 0; seg_num < 3; ++seg_num) {
    const Pong3Table::LedSegment* segment =
        table_->GetSegment(Pong3Table::ST_RADIAL,
			   (start_radial_ + 2 * seg_num) % 6);
    const int length = 1 + abs(segment->start - segment->end);
    const int dir = (segment->start < segment->end) ? 1 : -1;

    for (int i = 0; i < length; ++i) {
      int led = segment->start + i * dir;
      layer_->SetColor(led, Color(0, 0, 0));
    }
  }

  setting_ = (new_setting + num_settings) % num_settings;
  speed_ = (setting_ / 2) * 2 + 1;
  start_radial_ = setting_ % 2;

  return setting_;
}
