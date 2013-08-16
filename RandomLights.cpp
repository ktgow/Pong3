#include "RandomLights.h"

#include "ColorInterpolator.h"
#include "Constants.h"
#include "Pong3Table.h"

RandomLights::RandomLights(ColorInterpolator* colors, Pong3Table* table,
			   LedLayer* layer)
  : Animation(table, layer),
    colors_(colors),
    table_(table) {
}

RandomLights::~RandomLights() {
}

void RandomLights::Step() {
  for (int led = 0; led < NUM_LEDS; ++led) {
    table_->SetPixel(led, colors_->GetColor(random(64), 64));
  }
}
