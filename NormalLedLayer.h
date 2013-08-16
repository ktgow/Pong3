#ifndef _NORMALLEDLAYER_H_
#define _NORMALLEDLAYER_H_

#include "Color.h"
#include "Constants.h"
#include "LedLayer.h"

class NormalLedLayer : public LedLayer {
public:
  NormalLedLayer();
  virtual ~NormalLedLayer();

  virtual Color SetColor(int led, const Color& color);
  virtual Color GetColor(int led) const;
  virtual unsigned char GetLayerAlpha() const;

  // Blend another layer on top of this one, merging the results
  // into this layer.
  virtual void BlendLayer(const LedLayer* source);

  inline void SetLayerAlpha(unsigned char alpha) { layer_alpha_ = alpha; }

private:
  Color leds_[NUM_LEDS];
  unsigned char layer_alpha_;
};

#endif // _NORMALLEDLAYER_H_

