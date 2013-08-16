#include "NormalLedLayer.h"

#include "Util.h"

NormalLedLayer::NormalLedLayer() : layer_alpha_(255) {
  // Layers start all black, no alpha.
  //Serial.print("Size of led array: ");
  //Serial.println(sizeof(Color[NUM_LEDS]));
  //Serial.print("Available mem: ");
  //Serial.println(availableMemory());
}

NormalLedLayer::~NormalLedLayer() {
}

Color NormalLedLayer::SetColor(int led, const Color& color) {
  leds_[led] = color;
}

Color NormalLedLayer::GetColor(int led) const {
  return leds_[led];
}

unsigned char NormalLedLayer::GetLayerAlpha() const
{
  return layer_alpha_;
}

// Blend another layer on top of this one, merging the results
// into this layer.
void NormalLedLayer::BlendLayer(const LedLayer* source) {
  for (int led = 0; led < NUM_LEDS; ++led) {
    Color source_color = source->GetColor(led);
    /*
    Serial.print("Source color ");
    Serial.print(led);
    Serial.print(": ");
    Serial.print(source_color.red);
    Serial.print(", ");
    Serial.print(source_color.green);
    Serial.print(", ");
    Serial.print(source_color.blue);
    Serial.print(", ");
    Serial.println(source_color.alpha);
    */

    Color result =
      interp(GetColor(led), source_color,
	     (unsigned int)source_color.alpha * (unsigned int)source->GetLayerAlpha() / 255, 256);
    /*
    Serial.println((int)source_color.alpha);
    Serial.println((int)source->GetAlpha());
    Serial.println((unsigned int)source_color.alpha * (unsigned int)source->GetAlpha() / (unsigned int)255);
    Serial.println(interp(GetColor(led).blue, source_color.blue, (unsigned int)source_color.alpha * (int)source->GetAlpha() / 255, 256));
    */

    SetColor(led, result);
  }
}

