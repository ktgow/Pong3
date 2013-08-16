#ifndef _LEDLAYER_H_
#define _LEDLAYER_H_

#include "Color.h"
#include "Constants.h"

class LedLayer {
public:
  LedLayer();
  virtual ~LedLayer();

  virtual Color SetColor(int led, const Color& color) = 0;
  virtual Color GetColor(int led) const = 0;
  virtual unsigned char GetLayerAlpha() const { return 255; }

  // Blend another layer on top of this one, merging the results
  // into this layer.
  virtual void BlendLayer(const LedLayer* source) = 0;

};

#endif // _LEDLAYER_H_
