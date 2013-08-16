#ifndef _BASELEDLAYER_H_
#define _BASELEDLAYER_H_

#include "Color.h"
#include "LedLayer.h"

class Adafruit_NeoPixel;

class BaseLedLayer : public LedLayer {
public:
  BaseLedLayer(Adafruit_NeoPixel* strip);
  virtual ~BaseLedLayer();

  virtual Color SetColor(int led, const Color& color);
  virtual Color GetColor(int led) const;

  // Blend another layer on top of this one, merging the results
  // into this layer.
  virtual void BlendLayer(const LedLayer* source);

private:
  Adafruit_NeoPixel* strip_;
};

#endif // _BASELEDLAYER_H_
