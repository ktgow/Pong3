#ifndef _HEXAPONGTABLE_H_
#define _HEXAPONGTABLE_H_

#include "Arduino.h"
#include "Constants.h"
#include "Color.h"

class Adafruit_NeoPixel;
class LedLayer;

class Pong3Table {
public:
  enum SegmentType {
    ST_RADIAL,
    ST_BORDER,
    ST_NUM_TYPES
  };

  Pong3Table(Adafruit_NeoPixel* strip);

  // With any of these function, if no layer is specified, it will default to
  // the base layer.
  void SetSegmentColor(SegmentType segment_type, int segment_num,
		       Color color, LedLayer* layer = NULL);
  void SetSegmentSweep(SegmentType segment_type, int segment_num,
		       Color start_color, Color end_color,
		       LedLayer* layer = NULL);
  void SetPixel(int led, Color color, LedLayer* layer = NULL) const;

  // Merge and draw all layers.
  void Show() const;
  
  // Clear the table to black and delete all layers.
  void ClearAndDeleteLayers();

  LedLayer* AddLayer();
  LedLayer* GetBaseLayer() const { return layers_->layer; }

  struct LedSegment {
    int start;
    int end;
    
    LedSegment(int s, int e) : start(s), end(e) {}
  };

  const LedSegment* GetSegment(SegmentType segment_type,
			       int segment_num) const;

private:
  Adafruit_NeoPixel* strip_;

  LedSegment radial1_, radial2_, radial3_, radial4_, radial5_, radial6_;
  LedSegment border1_, border2_, border3_, border4_, border5_, border6_;
  LedSegment* segments_[ST_NUM_TYPES][6];

  struct LedLayerList {
    LedLayer* layer;
    LedLayerList* next;

    LedLayerList() : layer(NULL), next(NULL) {}
  };
  LedLayerList* layers_;

  friend class Runner;
};

#endif // _HEXAPONGTABLE_H_
