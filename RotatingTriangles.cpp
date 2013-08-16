#include "RotatingTriangles.h"

#include "ColorInterpolator.h"
#include "Pong3Table.h"
#include "LedLayer.h"
#include "LedPath.h"

RotatingTriangles::RotatingTriangles(unsigned long animation_time_ms,
				     ColorInterpolator* colors,
				     Pong3Table* table,
				     LedLayer* layer)
  : Animation(table, layer),
    animation_time_ms_(animation_time_ms),
    colors_(colors),
    time_start_ms_(0),
    triangles_(NULL),
    table_(table) {
  time_start_ms_ = millis();

  triangles_ = new LedPath[NUM_PATHS];
  for (int triangle = 0; triangle < NUM_PATHS; ++triangle) {
    const Pong3Table::LedSegment* segment =
      table_->GetSegment(Pong3Table::ST_RADIAL, triangle);
    triangles_[triangle].AddSegment(segment->start, segment->end);

    segment = table_->GetSegment(Pong3Table::ST_BORDER, triangle);
    triangles_[triangle].AddSegment(segment->start, segment->end);

    segment = table_->GetSegment(Pong3Table::ST_RADIAL, (triangle + 1) % 6);
    triangles_[triangle].AddSegment(segment->end, segment->start);
  }
}

RotatingTriangles::~RotatingTriangles() {
  if (triangles_ != NULL) delete[] triangles_;
}

void RotatingTriangles::Step() {
  unsigned long current_time_ms = millis();
  unsigned long rotation_step =
    ((current_time_ms - time_start_ms_) / animation_time_ms_)  % 6;

  // Brandon

  // Fill in the new triangles.
  for (int i = 0; i < 3; ++i) {
    const Color color = colors_->GetColor(i, 3);
    LedPath* triangle = &triangles_[(i * 2 + rotation_step) % 6];
    for (LedPath::Iterator iter = triangle->begin();
	 iter != triangle->end(); ++iter) {
      layer_->SetColor(*iter, color);
    }
  }

  // Clear the remaining sides.
  const Color black(0, 0, 0);
  for (int i = 0; i < 3; ++i) {
    const Pong3Table::LedSegment* segment =
      table_->GetSegment(Pong3Table::ST_BORDER, (i * 2 + rotation_step + 1) % 6);
    int direction = (segment->start < segment->end) ? 1 : -1;

    for (int led = segment->start; led != (segment->end + direction);
	 led += direction) {
      layer_->SetColor(led, black);
    }
  }
}

// Update to a new setting.  If the new setting is out of range, it should
// wrap to 0.  Returns the number of the new setting.
int RotatingTriangles::UpdateSetting(int new_setting) {
  switch (new_setting) {
    case 0:
    default:
      setting_ = 0;
      animation_time_ms_ = 500;
      break;

    case 1:
      setting_ = new_setting;
      animation_time_ms_ = 100;
      break;
  }

  return setting_;
}
