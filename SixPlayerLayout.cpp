#include "SixPlayerLayout.h"

#include "ColorInterpolator.h"
#include "Pong3Table.h"
#include "LedLayer.h"
#include "Util.h"

SixPlayerLayout::SixPlayerLayout(ColorInterpolator* colors,
				 int speed, bool spiral,
				 AnimationType anim_type,
				 Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    colors_(colors),
    speed_(speed),
    step_(0),
    spiral_(spiral),
    anim_type_(anim_type),
    table_(table) {
}

void SixPlayerLayout::Step() {
  ComputeCycleLength cycle_len;
  const Pong3Table::LedSegment* segments[6];
  for (int seg_num = 0; seg_num < 6; ++seg_num) {
    segments[seg_num] = table_->GetSegment(Pong3Table::ST_RADIAL, seg_num);
    const int length = 1 + abs(segments[seg_num]->start - segments[seg_num]->end);
    cycle_len.AddLength(length);
  }
  const int length_product = cycle_len.GetCycleLength();

  step_ = (step_ + speed_) % length_product;

  int inner_outer_dir = -1;
  if (anim_type_ == AT_OUTSIDE_TO_INSIDE) inner_outer_dir = 1;

  for (int seg_num = 0; seg_num < 6; ++seg_num) {
    const Pong3Table::LedSegment* segment = segments[seg_num];
    const int length = 1 + abs(segment->start - segment->end);
    const int dir = (segment->start < segment->end) ? 1 : -1;

    int color_offset = 0;
    if (spiral_) {
      color_offset = seg_num * length / 6;
    }

    for (int i = 0; i < length; ++i) {
      int led = segment->start + i * dir;
      layer_->SetColor(led,
		       colors_->GetColor((color_offset + inner_outer_dir *
					  step_ + i + length) % length,
					 length));
    }
  }
}

int SixPlayerLayout::UpdateSetting(int new_setting) {
  const int num_settings = 8;

  setting_ = (new_setting + num_settings) % num_settings;
  
  spiral_ = (setting_ % 2) == 0;
  anim_type_ = ((setting_ / 2) % 2 == 0) ? AT_OUTSIDE_TO_INSIDE :
    AT_INSIDE_TO_OUTSIDE;
  speed_ = setting_ / 4 * 2 + 1;

  return setting_;
}
