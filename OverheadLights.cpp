#include "OverheadLights.h"

#include "ColorInterpolator.h"

OverheadLights::OverheadLights(ColorInterpolator* colors, Pong3Table* table,
			       LedLayer* layer)
  : Animation(table, layer),
    colors_(colors),
    step_(0),
    color_is_locked_(false) {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

OverheadLights::~OverheadLights() {
}

void OverheadLights::Step() {
  if (color_is_locked_) {
    SetColor(locked_color_);
    return;
  }

  step_ = (step_ + 1) % NUM_STEPS;

  SetColor(colors_->GetColor(step_, NUM_STEPS));
}

int OverheadLights::UpdateSetting(int new_setting) {
  return 0;
}

void OverheadLights::SetColors(ColorInterpolator* colors) {
  color_is_locked_ = false;
  colors_ = colors;
}

void OverheadLights::SetColor(const Color& color) {
  analogWrite(RED_PIN, color.red);
  analogWrite(GREEN_PIN, color.green);
  analogWrite(BLUE_PIN, color.blue);
}

void OverheadLights::LockColor(const Color& color) {
  color_is_locked_ = true;
  locked_color_ = color;
}
