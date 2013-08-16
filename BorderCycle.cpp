#include "BorderCycle.h"

#include "ColorInterpolator.h"
#include "Pong3Table.h"
#include "LedLayer.h"

BorderCycle::BorderCycle(ColorInterpolator* border_colors,
			 ColorInterpolator* center_colors,
			 int speed,
			 Pong3Table* table,
			 LedLayer* layer)
  : Animation(table, layer),
    border_colors_(border_colors),
    center_colors_(center_colors),
    speed_(speed),
    table_(table),
    step_(0),
    num_steps_(0) {

  // Calc number of steps based on number of LEDs in all the border
  // segments.
  for (int segment_num = 0; segment_num < 6; ++segment_num) {
    const Pong3Table::LedSegment* segment = table_->GetSegment(
        Pong3Table::ST_BORDER, segment_num);
    num_steps_ += abs(segment->start - segment->end) + 1;
  }
}

void BorderCycle::Step() {
  step_ = (step_ + speed_) % num_steps_;
  if (step_ < 0) step_ += num_steps_;

  // Set lights across all the border LEDs.
  int color_offset = step_;
  for (int segment_num = 0; segment_num < 6; ++segment_num) {
    const Pong3Table::LedSegment* segment = table_->GetSegment(
        Pong3Table::ST_BORDER, segment_num);
    int direction = (segment->start < segment->end) ? 1 : -1;
    int num_leds = abs(segment->start - segment->end) + 1;
    for (int i = 0; i < num_leds; ++i) {
      int led = segment->start + (i * direction);
      layer_->SetColor(led, border_colors_->GetColor(color_offset, num_steps_));
      color_offset = (color_offset + 1) % num_steps_;
    }
  }

  // Set the lights on each of the radials.
  Color center_color = center_colors_->GetColor((step_ * 2) % num_steps_, num_steps_);
  for (int segment_num = 0; segment_num < 6; ++segment_num) {
    const Pong3Table::LedSegment* border_segment = table_->GetSegment(
        Pong3Table::ST_BORDER, segment_num);
    table_->SetSegmentSweep(Pong3Table::ST_RADIAL, segment_num,
			    center_color,
			    layer_->GetColor(border_segment->start),
			    layer_);
  }
}

int BorderCycle::UpdateSetting(int new_setting) {
  const int num_settings = 2;

  setting_ = (new_setting + num_settings) % num_settings;
  speed_ = setting_ * 2 + 1;

  return setting_;
}

