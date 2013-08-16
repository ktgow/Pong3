#include "JellyFish.h"

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "ColorInterpolator.h"
#include "LedLayer.h"
#include "Pong3Table.h"
#include "Util.h"

JellyFish::JellyFish(int max_length, ColorInterpolator* colors, int speed,
		     Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    max_length_(max_length),
    colors_(colors),
    speed_(speed),
    table_(table),
    stage_(MOVE_OUT),
    step_(0) {
}

void JellyFish::SetSegmentColors(int segment_type, int segment_num,
				 int start_fraction, int max_segment_len,
                                 int direction_multiplier) {
  if (start_fraction < 0) return;

  const Pong3Table::LedSegment* segment = table_->
    GetSegment((Pong3Table::SegmentType)segment_type, segment_num);
  int seg_start, seg_end;
  if (direction_multiplier == 1) {
    seg_start = segment->start;
    seg_end = segment->end;
  } else {
    seg_start = segment->end;
    seg_end = segment->start;
  }
    
  int segment_distance = 1 + abs(seg_start - seg_end);
  int extra_tail_dist =
    (start_fraction - speed_) * segment_distance / max_segment_len - max_length_;
  if (extra_tail_dist > segment_distance) return;

  int direction = (seg_start < seg_end) ? 1 : -1;
  int start_dist = start_fraction * segment_distance / max_segment_len;
  int end_dist = start_dist - max_length_;
  int start_led = seg_start + direction * start_dist;
  int end_led = seg_start + direction * end_dist;
  int extra_tail_led = seg_start + direction * extra_tail_dist;

/*
  Serial.print("sd ");
  Serial.print(start_dist);
  Serial.print(", ed ");
  Serial.print(end_dist);
  Serial.print(", start ");
  Serial.print(start_led);
  Serial.print(", end ");
  Serial.print(extra_tail_led);
  Serial.print(", direction ");
  Serial.println(direction);
*/

  for (int led = start_led;
       led != extra_tail_led - direction;
       led -= direction) {
    if (((led - seg_start) * direction < 0) ||
        (led - seg_start) * direction > (seg_end - seg_start) * direction) {
      continue;
    }
    layer_->SetColor(led, colors_->GetColor((start_led - led) * direction,
					    max_length_));
  }
}

void JellyFish::Step() {
  int radial_dist = FindLongestSegmentLength(table_, Pong3Table::ST_RADIAL);
  int border_dist = FindLongestSegmentLength(table_, Pong3Table::ST_BORDER);
  int total_dist = radial_dist * 2 + border_dist;

  // Outward radial.
  for (int segment_num = 0; segment_num < 6; ++segment_num) {
    SetSegmentColors(Pong3Table::ST_RADIAL, segment_num,
		     step_, radial_dist, 1);
  }
  // Across the border.
  for (int segment_num = 0; segment_num < 6; ++segment_num) {
    SetSegmentColors(Pong3Table::ST_BORDER, segment_num,
		     step_ - radial_dist, border_dist, 1);
  }
  // Inward radial.
  for (int segment_num = 0; segment_num < 6; ++segment_num) {
    SetSegmentColors(Pong3Table::ST_RADIAL, segment_num,
		     step_ - radial_dist - border_dist, radial_dist, -1);
  }

  if (step_ - max_length_ > total_dist) {
    step_ = 0;
  } else {
    step_ += speed_;
  }
}
