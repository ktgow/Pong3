#include "Pong3Table.h"
#include "Adafruit_NeoPixel.h"
#include "BaseLedLayer.h"
#include "NormalLedLayer.h"
#include "Util.h"

Pong3Table::Pong3Table(Adafruit_NeoPixel* strip) :
  strip_(strip),
  radial2_(34, 0),
  radial1_(35, 69),
  border1_(70, 104),
  border2_(105, 140),
  border3_(141, 175),
  radial4_(210, 176),
  radial3_(211, 245),
  border4_(246, 280),
  radial5_(315, 281),
  radial6_(316, 350),
  border5_(385, 351),
  border6_(386, 420) {
  segments_[ST_RADIAL][0] = &radial1_;
  segments_[ST_RADIAL][1] = &radial2_;
  segments_[ST_RADIAL][2] = &radial3_;
  segments_[ST_RADIAL][3] = &radial4_;
  segments_[ST_RADIAL][4] = &radial5_;
  segments_[ST_RADIAL][5] = &radial6_;
  segments_[ST_BORDER][0] = &border1_;
  segments_[ST_BORDER][1] = &border2_;
  segments_[ST_BORDER][2] = &border3_;
  segments_[ST_BORDER][3] = &border4_;
  segments_[ST_BORDER][4] = &border5_;
  segments_[ST_BORDER][5] = &border6_;
  
  layers_ = new LedLayerList;
  layers_->layer = new BaseLedLayer(strip_);
}

void Pong3Table::SetSegmentColor(SegmentType segment_type, int segment_num,
				    Color color, LedLayer* layer) {
  const LedSegment* segment = GetSegment(segment_type, segment_num);
  if (segment == NULL) return;

  int16_t step = (segment->start < segment->end) ? 1 : -1;
  for (uint16_t i = segment->start; ; i += step) {
    SetPixel(i, color, layer);
    if (i == segment->end)
      break;
  }
}

void Pong3Table::SetSegmentSweep(SegmentType segment_type, int segment_num,
				    Color start_color, Color end_color,
				    LedLayer* layer) {
  const LedSegment* segment = GetSegment(segment_type, segment_num);
  if (segment == NULL) return;

  int16_t direction = (segment->start < segment->end) ? 1 : -1;
  int num_steps = (segment->end - segment->start) * direction + 1;
  for (int step = 0; step < num_steps; ++step) {
    uint16_t led = segment->start + (step * direction);
    SetPixel(led, interp(start_color, end_color, step, num_steps), layer);
  }
}

void Pong3Table::SetPixel(int led, Color color, LedLayer* layer) const {
  // Default to the base layer.
  if (layer == NULL)
    layer = layers_->layer;

  layer->SetColor(led, color);
}

void Pong3Table::Show() const {
  // Merge all layers into a new draw layer.
  LedLayer* draw_layer = layers_->layer;
  LedLayerList* node = layers_->next;
  while (node != NULL) {
    draw_layer->BlendLayer(node->layer);
    node = node->next;
  }

  strip_->show();
}

void Pong3Table::ClearAndDeleteLayers() {
  LedLayer* draw_layer = layers_->layer;
  Color black(0, 0, 0);
  for (int led = 0; led < NUM_LEDS; ++led) {
    draw_layer->SetColor(led, black);
  }

  LedLayerList* node = layers_->next;
  while (node != NULL) {
    LedLayerList* prev = node;
    node = node->next;
    delete prev->layer;
    delete prev;
  }
  layers_->next = NULL;
}

LedLayer* Pong3Table::AddLayer() {
  // Add new layers to the top of the stack.
  LedLayerList* node = layers_;
  while (node->next != NULL) {
    node = node->next;
  }
  LedLayer* layer = new NormalLedLayer;
  if (layer != NULL) {
    node->next = new LedLayerList;
    if (node->next != NULL) {
      node = node->next;
      node->layer = layer;
    } else {
      delete layer;
      layer = NULL;
    }
  }
  return layer;
}

const Pong3Table::LedSegment* Pong3Table::GetSegment(
    SegmentType segment_type, int segment_num) const {
  if (segment_num >= 0 && segment_num < 6) {
    return segments_[segment_type][segment_num];
  }
  return NULL;
}
