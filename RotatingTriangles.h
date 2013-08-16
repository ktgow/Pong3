#ifndef _ROTATINGTRIANGLES_H_
#define _ROTATINGTRIANGLES_H_

#include "Animation.h"

class LedPath;

class RotatingTriangles : public Animation {
public:
  RotatingTriangles(unsigned long animation_time_ms,
		    ColorInterpolator* colors, Pong3Table* table,
		    LedLayer* layer = NULL);
  virtual ~RotatingTriangles();

  virtual void Step();

  // Update to a new setting.  If the new setting is out of range, it should
  // wrap to 0.  Returns the number of the new setting.
  virtual int UpdateSetting(int new_setting);
  virtual void SetColors(ColorInterpolator* colors) { colors_ = colors; }

private:
  enum { NUM_PATHS = 6 };

  ColorInterpolator* colors_;
  unsigned long time_start_ms_;
  unsigned long animation_time_ms_;
  LedPath* triangles_;
  Pong3Table* table_;
};

#endif // _ROTATINGTRIANGLES_H_
