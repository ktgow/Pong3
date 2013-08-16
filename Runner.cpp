#include "Runner.h"

#include "Arduino.h"
#include "keyFrameColorInterpolator.h"
#include "LedGraph.h"
#include "LedLayer.h"
#include "Pong3Table.h"
#include "Util.h"

Runner::Runner(int start_led, int max_length, ColorInterpolator* colors,
	       Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    start_led_(start_led),
    max_length_(max_length),
    speed_(1),
    colors_(colors),
    prev_leds_(max_length, start_led_),
    table_(table) {
}

Runner::~Runner() {
}

void Runner::Step() {
  for (int speed_step = 0; speed_step < speed_; ++speed_step) {
    // Choose the next LED.
    const LedGraph::GraphNode node = ledGraph.GetNode(start_led_);
    int chosen = 0;
    int available_so_far = 0;
    for (int neighbor = 0; neighbor < node.num_neighbors; ++neighbor) {
      bool skip = false;
      // Check against the last 5 LEDs to make sure we don't backtrack.
      for (int i = 0; i < min(max_length_, 6); ++i) {
        if (node.neighbors[neighbor] ==
	    prev_leds_.GetElement(max_length_ - 1 - i)) {
  	skip = true;
  	break;
        }
      }
      if (skip) continue;
  
      ++available_so_far;
      if (random(available_so_far) == 0) {
        chosen = node.neighbors[neighbor];
      }
    }
  
    // Move to the next LED.
    prev_leds_.AddElement(start_led_);
    start_led_ = chosen;
    
    // Draw the last LED in the trail, in case we're stepping multiple
    // times.
    layer_->SetColor(prev_leds_.GetElement(0),
                     colors_->GetColor(max_length_, max_length_));
  }

  // Draw the trail.  Start at 1 so we skip the end, since we already
  // drew that one.
  for (int i = 1; i < max_length_; ++i) {
    Color interp_color = colors_->GetColor(max_length_ - i, max_length_);
    layer_->SetColor(prev_leds_.GetElement(i), interp_color);
  }
}

int Runner::UpdateSetting(int new_setting) {
  const int num_settings = 3;

  setting_ = (new_setting + num_settings) % num_settings;

  switch (setting_) {
    case 0:
      speed_ = 1;
      break;
      
    case 1:
      speed_ = 4;
      break;
      
    case 2:
      speed_ = random(1, 7);
      break;
  }

  return setting_;
}

