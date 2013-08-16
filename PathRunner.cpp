#include "PathRunner.h"

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "keyFrameColorInterpolator.h"
#include "LedGraph.h"
#include "LedLayer.h"
#include "LedPath.h"
#include "Pong3Table.h"
#include "Util.h"

PathRunner::PathRunner(LedPath* path, int max_length, ColorInterpolator* colors,
		       Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    path_(path),
    start_idx_(0),
    max_length_(max_length),
    colors_(colors),
    prev_leds_start_(0),
    direction_(1) {
  prev_leds_ = new int[max_length_];
  // prev_leds_ saves led numbers rather than indices.
  int start_led = path->GetLed(0);
  for (int i = 0; i < max_length; ++i) {
    prev_leds_[i] = start_led;
  }
}

PathRunner::~PathRunner() {
  delete[] prev_leds_;
}

void PathRunner::Step() {
  int chosen_idx = start_idx_ + direction_;
  if (((direction_ > 0) && (chosen_idx >= path_->GetLength())) ||
      ((direction_ < 0) && (chosen_idx < 0))) {
    direction_ = -direction_;
    chosen_idx = start_idx_ + direction_;
  }
  int chosen_led = path_->GetLed(chosen_idx);

  // Move to the next LED.
  prev_leds_[prev_leds_start_] = chosen_led;
  prev_leds_start_ = (prev_leds_start_ + 1) % max_length_;
  start_idx_ = chosen_idx;

  // Draw the trail.
  for (int i = 0; i < max_length_; ++i) {
    Color interp_color = colors_->GetColor(max_length_ - i, max_length_);
    int led = path_->GetLed(prev_leds_[(prev_leds_start_ + i) % max_length_]);
    layer_->SetColor(led, interp_color);
  }
}

