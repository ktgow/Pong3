#ifndef _KEYFRAMECOLORINTERPOLATOR_H_
#define _KEYFRAMECOLORINTERPOLATOR_H_

#include "Color.h"
#include "ColorInterpolator.h"

class KeyFrameColorInterpolator : public ColorInterpolator {
public:
  struct KeyFrame {
    Color color;
    unsigned short distance; // Distance key, from 0 to 65535.
    enum { DIST_MAX = 0xffff };

    KeyFrame() : distance(0.0) {}
    KeyFrame(const Color& c, float d) : color(c), distance(d) {}
  };

  KeyFrameColorInterpolator();
  virtual ~KeyFrameColorInterpolator();

  void AddKeyFrame(KeyFrame frame);

  virtual Color GetColor(int step, int num_steps);

  void SetCacheEnabled(bool enabled);

private:
  struct KeyFrameList {
    KeyFrame frame;
    KeyFrameList* next;
  };

  KeyFrameList* frames_;

  enum { LOOKUP_SIZE = 64 };
  Color* lookup_;
  bool cache_enabled_;

  Color GetColorInternal(int step, int num_steps);
};

#endif // _KEYFRAMECOLORINTERPOLATOR_H_
