#include "Fire.h"

#include "CircularBuffer.h"
#include "ColorInterpolator.h"
#include "Globals.h"
#include "Pong3Table.h"
#include "LedLayer.h"
#include "LedPath.h"
#include "OverheadLights.h"

Fire::Fire(ColorInterpolator* colors, Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    step_(0),
    colors_(colors),
    fire_paths_(NULL),
    table_(table) {
  for (int radial = 0; radial < NUM_RADIALS; ++radial) {
    for (int i = 1; i < FIRE_LENGTH; ++i) {
      fire_color_[radial][i] = MAX_COLOR - 1;
    }
  }

  fire_paths_ = new LedPath[NUM_RADIALS];
  for (int radial = 0; radial < NUM_RADIALS; ++radial) {
    const Pong3Table::LedSegment* segment = table_->GetSegment(
        Pong3Table::ST_RADIAL, radial);
    fire_paths_[radial].AddSegment(segment->start, segment->end);

    const Pong3Table::LedSegment* border = table_->GetSegment(
        Pong3Table::ST_BORDER, radial);
    fire_paths_[radial].AddSegment(border->start, border->end);
  }
}

Fire::~Fire() {
  delete[] fire_paths_;
}

void Fire::Step() {
  const bool set_root = (step_ % 2 == 0);
  ++step_;

  unsigned short hidden_color_sum = 0;

  CircularBuffer history(HISTORY_SIZE);
  for (int radial = 0; radial < NUM_RADIALS; radial += 2) {
    if (set_root) {
      fire_color_[radial][0] = random(MAX_COLOR);
      fire_color_[radial][1] = fire_color_[radial][0];
    }
    hidden_color_sum += fire_color_[radial][0];

    unsigned short sum = fire_color_[radial][0];
    history.AddElement(sum);
    unsigned short num_in_sum = 1;

    for (int i = 1; i < FIRE_LENGTH; ++i) {
      unsigned char fire_value = fire_color_[radial][i];

      sum += fire_value;
      ++num_in_sum;
      if (num_in_sum > HISTORY_SIZE) {
	sum -= history.GetElement(0);
	--num_in_sum;
      }
      history.AddElement(fire_value);

      unsigned short weighted_sum =
        (unsigned long)(sum - fire_value) * (unsigned long)num_in_sum / (unsigned long)(num_in_sum - 1);
      fire_color_[radial][i] = min(MAX_COLOR - 1, sum / num_in_sum + 1);
    }

    LedPath* path = &fire_paths_[radial];
    int fire_idx = NUM_HIDDEN;
    bool increment_fire = false;
    for (LedPath::Iterator iter = path->begin();
	 iter != path->end() && fire_idx < FIRE_LENGTH;
	 ++iter) {
      layer_->SetColor(*iter,
		       colors_->GetColor(fire_color_[radial][fire_idx],
					 MAX_COLOR));
      if (increment_fire) {
        ++fire_idx;
      }
      increment_fire = !increment_fire;
    }
    
    // Copy the borders to the other direction, too.
    const Pong3Table::LedSegment* left_border = table_->GetSegment(Pong3Table::ST_BORDER,
                                                                      (radial + 5) % 6);
    int left_dir = (left_border->start < left_border->end) ? 1 : -1;
    int num_leds = 1 + abs(left_border->start - left_border->end);
    const Pong3Table::LedSegment* right_border = table_->GetSegment(Pong3Table::ST_BORDER, radial);
    int right_dir = (right_border->start < right_border->end) ? 1 : -1;
    num_leds = min(num_leds, 1 + abs(right_border->start - right_border->end));

    for (int i = 0; i < num_leds; ++i) {
      int src_led = right_border->start + i * right_dir;
      int dst_led = left_border->end - i * left_dir;
      layer_->SetColor(dst_led, layer_->GetColor(src_led));
    }
  }

  // Set the overhead lights based on the average of the internal color.
  overhead_lights->
    LockColor(colors_->GetColor(hidden_color_sum / 3, MAX_COLOR));
}

