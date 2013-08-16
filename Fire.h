#ifndef _FIRE_H_
#define _FIRE_H_

#include "Animation.h"
#include "Color.h"

class LedPath;

class Fire : public Animation {
public:
  Fire(ColorInterpolator* colors, Pong3Table* table, LedLayer* layer = NULL);
  virtual ~Fire();

  virtual void Step();
  // Update to a new setting.  If the new setting is out of range, it should
  // wrap to 0.  Returns the number of the new setting.
  virtual int UpdateSetting(int new_setting) { return 0; }
  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

protected:
  enum { NUM_HIDDEN = 2,
	 FIRE_LENGTH = 64 + NUM_HIDDEN,
	 NUM_RADIALS = 6,
	 MAX_COLOR = 64,
	 HISTORY_SIZE = 2 };  // The number of color indices.

  unsigned int step_;
  unsigned char fire_color_[NUM_RADIALS][FIRE_LENGTH];
  ColorInterpolator* colors_;
  LedPath* fire_paths_;
  Pong3Table* table_;
};

#endif // _FIRE_H_

