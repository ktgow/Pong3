#include "Animation.h"
#include "Pong3Table.h"
#include "LedLayer.h"

Animation::Animation(Pong3Table* table, LedLayer* layer)
  : setting_(0) {
  //Serial.print("Animation layer in ");
  //Serial.println((int)layer);

  if (layer == NULL) {
    layer = table->AddLayer();
  }
  layer_ = layer;

  //Serial.print("Animation layer set ");
  //Serial.println((int)layer);
}

Animation::~Animation() {
}
