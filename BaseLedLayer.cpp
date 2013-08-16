#include "BaseLedLayer.h"

#include "Adafruit_NeoPixel.h"
#include "Util.h"

BaseLedLayer::BaseLedLayer(Adafruit_NeoPixel* strip) : strip_(strip) {
}

BaseLedLayer::~BaseLedLayer() {
}

Color BaseLedLayer::SetColor(int led, const Color& color) {
  if (color.alpha == 255) {
    strip_->setPixelColor(led, strip_->Color(color.red, color.green,
					     color.blue));
  } else {
    Color dest = GetColor(led);
    Color result = Color((unsigned int)dest.red * (unsigned int)(255 - color.alpha) / 255 +
                         (unsigned int)color.red * (unsigned int)(color.alpha) / 255,
                         (unsigned int)dest.green * (unsigned int)(255 - color.alpha) / 255 +
                         (unsigned int)color.green * (unsigned int)(color.alpha) / 255,
                         (unsigned int)dest.blue * (unsigned int)(255 - color.alpha) / 255 +
                         (unsigned int)color.blue * (unsigned int)(color.alpha) / 255);
    strip_->setPixelColor(led, strip_->Color(result.red, result.green, result.blue));
  }
}

Color BaseLedLayer::GetColor(int led) const {
  uint32_t value = strip_->getPixelColor(led);
  Color result((value >> 16) & 0xff,
	       (value >> 8) & 0xff,
	       value & 0xff,
	       255);
  return result;
}

// Blend another layer on top of this one, merging the results
// into this layer.
void BaseLedLayer::BlendLayer(const LedLayer* source) {
  for (int led = 0; led < NUM_LEDS; ++led) {
    Color source_color = source->GetColor(led);
    Color result =
      interp(GetColor(led), source_color,
	     (unsigned int)source_color.alpha *
	     (unsigned int)source->GetLayerAlpha() / 255, 256);
    SetColor(led, result);
  }
}
