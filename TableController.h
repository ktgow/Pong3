#ifndef _TABLECONTROLLER_H_
#define _TABLECONTROLLER_H_

#include "Color.h"
#include "ColorSet.h"
#include "Pong3Table.h"

class Animation;
class ColorInterpolator;
class LedPath;

// Control which animations and patterns are showing the table.
class TableController {
public:
  TableController(Pong3Table* table);
  ~TableController();

  // Cycle through overall patterns.
  void NextPattern();

  // Cycle through specific settings within the current pattern.
  void NextSetting();

  // Cycle through colors.
  void NextColorSet();

  // Back to the previous setting.
  void Back();

  void Step();

private:
  enum { MAX_HISTORY = 32 };
  struct Configuration {
    int pattern;
    int setting;
    int color;

    bool operator==(const Configuration& other) {
      return (other.pattern == pattern &&
	      other.setting == setting &&
	      other.color == color);
    }
  } configuration_history_[MAX_HISTORY];
  int current_config_;

  Pong3Table* table_;
  Animation** animations_;
  int num_animations_;
  ColorSet color_set_;
  LedPath** paths_;
  int num_paths_;

  enum { NUM_PATTERNS = 13,
         NUM_STD_COLORS = 7 };


  struct PathInfo {
    Pong3Table::SegmentType segment_type;
    int segment_num;
    bool forward;
    bool end_path;
  };

  void PrintConfig();
  void StopPattern();
  void CreateStandardColors();
  void CreatePaths(const PathInfo* path_info, int path_info_size);
  void SetupPattern(int pattern_num);
  void UpdateSetting(int setting_num);
  void PushConfiguration();
  void PopIfUnchanged();
  void UpdateColorSet(int color_num);
};

#endif // _TABLECONTROLLER_H_
