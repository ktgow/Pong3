#include "TwoPlayerLayout.h"

#include "ColorInterpolator.h"
#include "Pong3Table.h"
#include "LedLayer.h"

TwoPlayerLayout::TwoPlayerLayout(ColorInterpolator* colors, int speed,
				 int start_radial, AnimationType anim_type,
				 Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    colors_(colors),
    speed_(speed),
    step_(0),
    start_radial_(start_radial),
    anim_type_(anim_type),
    table_(table),
    setting_(0) {
}

void TwoPlayerLayout::Step() {
  const Pong3Table::LedSegment* radial1 = table_->GetSegment(
      Pong3Table::ST_RADIAL, start_radial_);
  const Pong3Table::LedSegment* radial2 = table_->GetSegment(
      Pong3Table::ST_RADIAL, (start_radial_ + 3) % 6);
  int radial1_length = 1 + abs(radial1->start - radial1->end);
  int radial1_dir = (radial1->start < radial1->end) ? 1 : -1;
  int radial2_length = 1 + abs(radial2->start - radial2->end);
  int radial2_dir = (radial2->start < radial2->end) ? 1 : -1;
  int num_leds = radial1_length + radial2_length;

  step_ = (step_ + speed_) % num_leds;

  int inner_outer_dir = -1;
  switch (anim_type_) {
    case AT_SWEEP_ACROSS:
      for (int i = 0; i < num_leds; ++i) {
	int led = 0;
	if (i < radial1_length) {
	  led = radial1->end - i * radial1_dir;
	} else {
	  led = radial2->start + (i - radial1_length) * radial2_dir;
	}

	layer_->SetColor(led, colors_->GetColor((step_ + i) % num_leds,
						num_leds));
      }
      break;

    case AT_OUTSIDE_TO_INSIDE:
      inner_outer_dir = 1;
      // FALLTHROUGH
    case AT_INSIDE_TO_OUTSIDE:
      for (int i = 0; i < radial1_length; ++i) {
	int led = radial1->start + i * radial1_dir;
	layer_->SetColor(
	    led,
	    colors_->GetColor((inner_outer_dir * step_ + i + num_leds) % num_leds,
			      num_leds));
      }
      for (int i = 0; i < radial2_length; ++i) {
	int led = radial2->start + i * radial2_dir;
	layer_->SetColor(
            led,
	    colors_->GetColor((inner_outer_dir * step_ + i + num_leds) % num_leds,
			      num_leds));
      }
      break;
  }
}

int TwoPlayerLayout::UpdateSetting(int new_setting) {
  const int num_settings = 6;

  // Set current leds to black.
  const Pong3Table::LedSegment* radial1 = table_->GetSegment(
      Pong3Table::ST_RADIAL, start_radial_);
  const Pong3Table::LedSegment* radial2 = table_->GetSegment(
      Pong3Table::ST_RADIAL, (start_radial_ + 3) % 6);
  int radial1_length = 1 + abs(radial1->start - radial1->end);
  int radial1_dir = (radial1->start < radial1->end) ? 1 : -1;
  int radial2_length = 1 + abs(radial2->start - radial2->end);
  int radial2_dir = (radial2->start < radial2->end) ? 1 : -1;
  int num_leds = radial1_length + radial2_length;

  for (int i = 0; i < radial1_length; ++i) {
    int led = radial1->start + i * radial1_dir;
    layer_->SetColor(led, Color(0, 0, 0));
  }
  for (int i = 0; i < radial2_length; ++i) {
    int led = radial2->start + i * radial2_dir;
    layer_->SetColor(led, Color(0, 0, 0));
  }

  setting_ = (new_setting + num_settings) % num_settings;
  speed_ = (setting_ / 3) * 2 + 1;
  start_radial_ = setting_ % 3;

  return setting_;
}
