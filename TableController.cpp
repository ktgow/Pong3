#include "TableController.h"

#include "AnimatePaths.h"
#include "Arduino.h"
#include "BorderCycle.h"
#include "ColorInterpolator.h"
#include "Fire.h"
#include "Globals.h"
#include "Pong3Table.h"
#include "KeyFrameColorInterpolator.h"
#include "JellyFish.h"
#include "LedPath.h"
#include "OverheadLights.h"
#include "RandomLights.h"
#include "RotatingTriangles.h"
#include "Runner.h"
#include "SixPlayerLayout.h"
#include "TwoPlayerLayout.h"
#include "ThreePlayerLayout.h"
#include "Util.h"

TableController::TableController(Pong3Table* table)
  : table_(table),
    current_config_(0),
    animations_(NULL),
    num_animations_(0),
    paths_(NULL),
    num_paths_(0) {
  memset(configuration_history_, 0, sizeof(configuration_history_));
  SetupPattern(0);
}

TableController::~TableController() {
  StopPattern();
}

void TableController::PrintConfig() {
  Configuration* config = &configuration_history_[current_config_];

  Serial.print("Configuration ");
  Serial.print(current_config_);
  Serial.print(": Pattern ");
  Serial.print(config->pattern);
  Serial.print(", Setting ");
  Serial.print(config->setting);
  Serial.print(", Color ");
  Serial.println(config->color);
}

void TableController::NextPattern() {
  PushConfiguration();
  SetupPattern((configuration_history_[current_config_].pattern + 1) %
	       NUM_PATTERNS);
  UpdateSetting(configuration_history_[current_config_].setting);
  UpdateColorSet(configuration_history_[current_config_].color);
  PopIfUnchanged();
  PrintConfig();
}

void TableController::NextSetting() {
  PushConfiguration();
  UpdateSetting(configuration_history_[current_config_].setting + 1);
  PopIfUnchanged();
  PrintConfig();
}

void TableController::NextColorSet() {
  PushConfiguration();
  UpdateColorSet(configuration_history_[current_config_].color + 1);
  PopIfUnchanged();
  PrintConfig();
}

void TableController::Back() {
  if (current_config_ <= 0) return;

  --current_config_;
  Configuration* config = &configuration_history_[current_config_];
  Configuration* prev_config = &configuration_history_[current_config_ + 1];

  bool change_pattern = (config->pattern != prev_config->pattern);
  bool change_setting = (config->setting != prev_config->setting);
  bool change_color = (config->color != prev_config->color);

  Configuration config_backup;
  memcpy(&config_backup, config, sizeof(Configuration));
  if (change_pattern) {
    // Changing the pattern always requires updating the setting and color.
    SetupPattern(config_backup.pattern);
    UpdateSetting(config_backup.setting);
    UpdateColorSet(config_backup.color);
  } else {
    if (change_setting) {
      UpdateSetting(config_backup.setting);
    }
    if (change_color) {
      UpdateColorSet(config_backup.color);
    }
  }
  PrintConfig();
}

void TableController::Step() {
  for (int i = 0; i < num_animations_; ++i) {
    animations_[i]->Step();
  }
}

void TableController::StopPattern() {
  Serial.print("Available mem before stopping pattern: ");
  Serial.println(availableMemory());

  if (animations_ != NULL) {
    for (int i = 0; i < num_animations_; ++i) {
      delete animations_[i];
    }
    delete[] animations_;
    animations_ = NULL;
    num_animations_ = 0;
  }
  color_set_.DeleteAll();
  if (paths_ != NULL) {
    for (int i = 0; i < num_paths_; ++i) {
      delete paths_[i];
    }
    delete[] paths_;
    paths_ = NULL;
    num_paths_ = 0;
  }
  table_->ClearAndDeleteLayers();
  
  Serial.print("Available mem after stopping pattern: ");
  Serial.println(availableMemory());
}

// Create a repeating color pattern between the two colors.
static ColorInterpolator* CreateRepeating(const Color& color1,
					  const Color& color2,
					  int repetitions) {
  KeyFrameColorInterpolator* repeating = new KeyFrameColorInterpolator;
  for (int step = 0; step < repetitions; ++step) {
    repeating->AddKeyFrame(
        KeyFrameColorInterpolator::KeyFrame(
          color1,
          ((float)(step * 2) / (float)(repetitions * 2)) *
	      KeyFrameColorInterpolator::KeyFrame::DIST_MAX));
    repeating->AddKeyFrame(
        KeyFrameColorInterpolator::KeyFrame(
          color2,
          (((float)(step * 2) + 1.0) / (float)(repetitions * 2)) *
	      KeyFrameColorInterpolator::KeyFrame::DIST_MAX));
  }
  repeating->AddKeyFrame(
      KeyFrameColorInterpolator::KeyFrame(
        color1,
	KeyFrameColorInterpolator::KeyFrame::DIST_MAX));

  return repeating;
}

void TableController::CreateStandardColors() {
  ColorSet::ColorKeyframeInfo trails[] = {
    { Color(30, 0, 0), 0.0 }, // Rainbow
    { Color(30, 30, 0), 1.0 / 6.0 },
    { Color(0, 30, 0), 2.0 / 6.0 },
    { Color(0, 30, 30), 3.0 / 6.0},
    { Color(0, 0, 30), 4.0 / 6.0},
    { Color(30, 0, 30), 5.0 / 6.0},
    { Color(30, 0, 0), 1.0 },

    { Color(30, 0, 0), 0.0 }, // Separated rainbow
    { Color(0, 0, 0), 1.0 / 12.0 },
    { Color(30, 30, 0), 2.0 / 12.0 },
    { Color(0, 0, 0), 3.0 / 12.0 },
    { Color(0, 30, 0), 4.0 / 12.0 },
    { Color(0, 0, 0), 5.0 / 12.0 },
    { Color(0, 30, 30), 6.0 / 12.0},
    { Color(0, 0, 0), 7.0 / 12.0 },
    { Color(0, 0, 30), 8.0 / 12.0},
    { Color(0, 0, 0), 9.0 / 12.0 },
    { Color(30, 0, 30), 10.0 / 12.0},
    { Color(0, 0, 0), 11.0 / 12.0 },
    { Color(30, 0, 0), 1.0 },

    { Color(0, 0, 30), 0}, // Blue green blue
    { Color(0, 30, 0), 0.5},
    { Color(0, 0, 30), 1.0},

    { Color(30, 30, 0), 0}, // Gold blue gold
    { Color(0, 0, 30), 0.5},
    { Color(30, 30, 0), 1.0},

    { Color(30, 0, 0), 0}, // Red green red
    { Color(0, 30, 0), 0.5},
    { Color(30, 0, 0), 1.0},

    { Color(0, 0, 0), 0}, // Black red black
    { Color(30, 0, 0), 0.5},
    { Color(0, 0, 0), 1.0},
 };

  color_set_.AddColors(trails, sizeof(trails) / sizeof(trails[0]));
  color_set_.AddColors(CreateRepeating(Color(0, 0, 0),
				       Color(30, 30, 30),
				       6));
  color_set_.AddColors(CreateRepeating(Color(0, 0, 0),
				       Color(30, 15, 0),
				       6));
}

void TableController::CreatePaths(const PathInfo* path_info, int path_info_size) {
  num_paths_ = 0;
  for (int i = 0; i < path_info_size; ++i) {
    if (path_info[i].end_path) {
      ++num_paths_;
    }
  }

  paths_ = new LedPath*[num_paths_];
  int path_num = 0;

  LedPath* path = NULL;
  for (int i = 0; i < path_info_size; ++i) {
    if (path == NULL) {
      path = new LedPath;
    }
    
    const Pong3Table::LedSegment* segment =
      table_->GetSegment(path_info[i].segment_type,
			 path_info[i].segment_num);
    path->AddSegment(path_info[i].forward ? segment->start : segment->end,
		     path_info[i].forward ? segment->end : segment->start);
    
    if (path_info[i].end_path) {
      paths_[path_num++] = path;
      path = NULL;
    }
  }
}

void TableController::SetupPattern(int pattern_num) {
  StopPattern();
  Configuration* config = &configuration_history_[current_config_];
  config->pattern = pattern_num;

  switch (pattern_num) {
    case 0: // Two player.
      {
	CreateStandardColors();

	num_paths_ = 1;
	paths_ = new LedPath*[num_paths_];
	int path_num = 0;

	LedPath* border_path = new LedPath;
	for (int i = 0; i < 6; ++i) {
	  const Pong3Table::LedSegment* segment =
	    table_->GetSegment(Pong3Table::ST_BORDER, i);
	  border_path->AddSegment(segment->start, segment->end);
	}
	paths_[path_num++] = border_path;

	num_animations_ = 2;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;

	animations_[anim_num++] =
	  new TwoPlayerLayout(color_set_.GetColor(0), 1,
			      0, TwoPlayerLayout::AT_INSIDE_TO_OUTSIDE,
			      table_, table_->GetBaseLayer());
	AnimatePaths* animate_border =
	  new AnimatePaths(AnimatePaths::AT_CYCLE, color_set_.GetColor(0), 3,
			   table_,
			   table_->GetBaseLayer());
	animate_border->AddPath(border_path);
	animations_[anim_num++] = animate_border;
      }
      overhead_lights->SetColors(overhead_colors.GetColor(0));
      break;

    case 1: // Three player.
      {
	CreateStandardColors();

	num_paths_ = 1;
	paths_ = new LedPath*[num_paths_];
	int path_num = 0;

	LedPath* border_path = new LedPath;
	for (int i = 0; i < 6; ++i) {
	  const Pong3Table::LedSegment* segment =
	    table_->GetSegment(Pong3Table::ST_BORDER, i);
	  border_path->AddSegment(segment->start, segment->end);
	}
	paths_[path_num++] = border_path;

	num_animations_ = 2;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	animations_[anim_num++] =
	  new ThreePlayerLayout(color_set_.GetColor(0), 1, 0,
				ThreePlayerLayout::AT_OUTSIDE_TO_INSIDE,
				table_, table_->GetBaseLayer());
	AnimatePaths* animate_border =
	  new AnimatePaths(AnimatePaths::AT_CYCLE, color_set_.GetColor(0), 3, table_,
			   table_->GetBaseLayer());
	animate_border->AddPath(border_path);
	animations_[anim_num++] = animate_border;
      }
      overhead_lights->SetColors(overhead_colors.GetColor(1));
      break;

    case 2: // Six player.
      {
	CreateStandardColors();

	num_paths_ = 1;
	paths_ = new LedPath*[num_paths_];
	int path_num = 0;

	LedPath* border_path = new LedPath;
	for (int i = 0; i < 6; ++i) {
	  const Pong3Table::LedSegment* segment =
	    table_->GetSegment(Pong3Table::ST_BORDER, i);
	  border_path->AddSegment(segment->start, segment->end);
	}
	paths_[path_num++] = border_path;

	num_animations_ = 2;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;

	animations_[anim_num++] =
	  new SixPlayerLayout(color_set_.GetColor(0), 1, true,
			      SixPlayerLayout::AT_INSIDE_TO_OUTSIDE,
			      table_, table_->GetBaseLayer());

	AnimatePaths* animate_border =
	  new AnimatePaths(AnimatePaths::AT_CYCLE, color_set_.GetColor(0), 3,
			   table_,
			   table_->GetBaseLayer());
	animate_border->AddPath(border_path);
	animations_[anim_num++] = animate_border;
      }
      overhead_lights->SetColors(overhead_colors.GetColor(0));
      break;

    case 3: // Peace symbol.
      {
	CreateStandardColors();

	PathInfo path_info[] = {
	  { Pong3Table::ST_RADIAL, 0, false, true },
	  { Pong3Table::ST_RADIAL, 1, false, true },
	  { Pong3Table::ST_RADIAL, 5, false, true },
	  { Pong3Table::ST_RADIAL, 3, true, true },
	  { Pong3Table::ST_BORDER, 3, true, false },
	  { Pong3Table::ST_BORDER, 4, true, false },
	  { Pong3Table::ST_BORDER, 5, true, true },
	  { Pong3Table::ST_BORDER, 2, false, false },
	  { Pong3Table::ST_BORDER, 1, false, false },
	  { Pong3Table::ST_BORDER, 0, false, true } };
	CreatePaths(path_info, sizeof(path_info) / sizeof(path_info[0]));

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	AnimatePaths* animate_path =
	  new AnimatePaths(AnimatePaths::AT_CYCLE, color_set_.GetColor(0), 1,
			   table_,
			   table_->GetBaseLayer());
	for (int i = 0; i < num_paths_; ++i) {
	  animate_path->AddPath(paths_[i]);
	}
	animations_[anim_num++] = animate_path;
      }
      overhead_lights->SetColors(overhead_colors.GetColor(0));
      break;

    case 4: // Jellyfish
      {
	CreateStandardColors();

	num_paths_ = 0;
	paths_ = NULL;

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;

	animations_[anim_num++] =
	  new JellyFish(20, color_set_.GetColor(0), 1, table_,
			table_->GetBaseLayer());
      }
      overhead_lights->SetColors(overhead_colors.GetColor(2));
      break;

    case 5: // Border cycle
      {
	CreateStandardColors();

	num_paths_ = 0;
	paths_ = NULL;

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	animations_[anim_num++] = new BorderCycle(color_set_.GetColor(0),
						  color_set_.GetColor(5), 1,
						  table_, table_->GetBaseLayer());
      }
      overhead_lights->SetColors(overhead_colors.GetColor(0));
      break;

    case 6: // Runners.
      {
	ColorSet::ColorKeyframeInfo trails[] = {
	  { Color(30, 30, 30), 0.0 },  // Blue to black
	  { Color(30, 30, 0), 0.1 },
	  { Color(0, 0, 30), 0.3 },
	  { Color(0, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Red to black
	  { Color(30, 30, 0), 0.1 },
	  { Color(30, 0, 0), 0.3 },
	  { Color(0, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Green to black
	  { Color(0, 30, 30), 0.1 },
	  { Color(0, 30, 0), 0.3 },
	  { Color(0, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Red to Blue
	  { Color(30, 30, 0), 0.1 },
	  { Color(30, 0, 0), 0.3 },
	  { Color(0, 0, 10), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Blue to Green
	  { Color(30, 0, 30), 0.1 },
	  { Color(0, 0, 30), 0.3 },
	  { Color(0, 10, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Green to Red
	  { Color(30, 30, 0), 0.1 },
	  { Color(0, 30, 0), 0.3 },
	  { Color(10, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // White to Red
	  { Color(30, 0, 0), 0.3 },
	  { Color(10, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // White to Yellow
	  { Color(30, 30, 0), 0.3 },
	  { Color(10, 10, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // White to Green
	  { Color(0, 30, 0), 0.3 },
	  { Color(0, 10, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // White to Light Blue
	  { Color(0, 30, 30), 0.3 },
	  { Color(0, 10, 10), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // White to Blue
	  { Color(0, 0, 30), 0.3 },
	  { Color(0, 0, 10), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // White to Purple
	  { Color(30, 0, 30), 0.3 },
	  { Color(10, 0, 10), 1.0 },
	};
	const int trails_size = sizeof(trails) / sizeof(trails[0]);
	color_set_.AddColors(trails, trails_size);

	num_paths_ = 0;
	paths_ = NULL;

	num_animations_ = 6;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	animations_[anim_num++] = new Runner(0, 20, color_set_.GetColor(0),
					     table_, table_->GetBaseLayer());
	animations_[anim_num++] = new Runner(69, 20, color_set_.GetColor(1),
					     table_, table_->GetBaseLayer());
	animations_[anim_num++] = new Runner(176, 20, color_set_.GetColor(2),
					     table_, table_->GetBaseLayer());
	animations_[anim_num++] = new Runner(245, 20, color_set_.GetColor(0),
					     table_, table_->GetBaseLayer());
	animations_[anim_num++] = new Runner(281, 20, color_set_.GetColor(1),
					     table_, table_->GetBaseLayer());
	animations_[anim_num++] = new Runner(350, 20, color_set_.GetColor(2),
					     table_, table_->GetBaseLayer());
      }
      overhead_lights->SetColors(overhead_colors.GetColor(3));
      break;

    case 7: // Biohazard
      {
	ColorSet::ColorKeyframeInfo color_keys[] = {
	  { Color(30, 30, 0), 0.0 },  // Yellow to black
	  { Color(0, 0, 0), 0.5 },
	  { Color(30, 30, 0), 1.0 },

 	  { Color(30, 30, 0), 0.0 },  // Yellow to red
	  { Color(30, 0, 0), 0.5 },
 	  { Color(30, 30, 0), 1.0 },

	  { Color(0, 30, 0), 0.0 },  // Green to black
	  { Color(0, 0, 0), 0.5 },
	  { Color(0, 30, 0), 1.0 },

	  { Color(0, 30, 0), 0.0 },  // Green to yellow
	  { Color(30, 30, 0), 0.5 },
	  { Color(0, 30, 0), 1.0 },

	  { Color(30, 15, 0), 0.0 },  // Orange to black
	  { Color(0, 0, 0), 0.5 },
	  { Color(30, 15, 0), 1.0 },

	  { Color(30, 0, 30), 0.0 },  // Purple to black
	  { Color(0, 0, 0), 0.5 },
	  { Color(30, 0, 30), 1.0 },

	  { Color(0, 0, 30), 0.0 },  // Blue to black
	  { Color(0, 0, 0), 0.5 },
	  { Color(0, 0, 30), 1.0 },
	};
	const int color_keys_size = sizeof(color_keys) / sizeof(color_keys[0]);
	color_set_.AddColors(color_keys, color_keys_size);

	PathInfo path_info[] = {
	  { Pong3Table::ST_RADIAL, 0, true, false },
	  { Pong3Table::ST_BORDER, 0, true, false },
	  { Pong3Table::ST_RADIAL, 1, false, true },

	  { Pong3Table::ST_RADIAL, 2, true, false },
	  { Pong3Table::ST_BORDER, 2, true, false },
	  { Pong3Table::ST_RADIAL, 3, false, true },

	  { Pong3Table::ST_RADIAL, 4, true, false },
	  { Pong3Table::ST_BORDER, 4, true, false },
	  { Pong3Table::ST_RADIAL, 5, false, true } };
	CreatePaths(path_info, sizeof(path_info) / sizeof(path_info[0]));

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	AnimatePaths* animate_path =
	  new AnimatePaths(AnimatePaths::AT_CYCLE, color_set_.GetColor(0), 1,
			   table_,
			   table_->GetBaseLayer());
	for (int i = 0; i < num_paths_; ++i) {
	  animate_path->AddPath(paths_[i]);
	}
	animations_[anim_num++] = animate_path;
      }
      overhead_lights->SetColors(overhead_colors.GetColor(4));
      break;

    case 8: // Fire.
      {
	ColorSet::ColorKeyframeInfo colors[] = {
	  { Color(30, 30, 30), 0.0 },  // Yellow red fire
	  { Color(30, 30, 0), 0.5 },
	  { Color(30, 0, 0), 0.75 },
	  { Color(0, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Blue red fire
	  { Color(0, 0, 30), 0.5 },
	  { Color(30, 0, 0), 0.75 },
	  { Color(0, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Blue green fire
	  { Color(0, 30, 0), 0.5 },
	  { Color(0, 0, 30), 0.75 },
	  { Color(0, 0, 0), 1.0 },

	  { Color(30, 0, 0), 0.0 }, // Rainbow
          { Color(30, 30, 0), 1.0 / 6.0 },
          { Color(0, 30, 0), 2.0 / 6.0 },
          { Color(0, 30, 30), 3.0 / 6.0},
          { Color(0, 0, 30), 4.0 / 6.0},
          { Color(30, 0, 30), 5.0 / 6.0},
          { Color(0, 0, 0), 1.0 },
	};
	const int colors_size = sizeof(colors) / sizeof(colors[0]);
	color_set_.AddColors(colors, colors_size);

	num_paths_ = 0;
	paths_ = NULL;

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	animations_[anim_num++] = new Fire(color_set_.GetColor(0),
					   table_, table_->GetBaseLayer());
      }
      // Fire effect will control the overhead lights directly.
      //overhead_lights->SetColors(overhead_colors.GetColor(5));
      break;

    case 9: // Random static
      {
	CreateStandardColors();

	num_paths_ = 0;
	paths_ = NULL;

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	animations_[anim_num++] = new RandomLights(color_set_.GetColor(0),
						   table_,
						   table_->GetBaseLayer());
      }
      overhead_lights->SetColors(overhead_colors.GetColor(1));
      break;

    case 10: // Every segment
      {
	CreateStandardColors();

	PathInfo path_info[] = {
	  { Pong3Table::ST_RADIAL, 0, true, true },
	  { Pong3Table::ST_RADIAL, 1, true, true },
	  { Pong3Table::ST_RADIAL, 2, true, true },
	  { Pong3Table::ST_RADIAL, 3, true, true },
	  { Pong3Table::ST_RADIAL, 4, true, true },
	  { Pong3Table::ST_RADIAL, 5, true, true },
	  { Pong3Table::ST_BORDER, 0, true, true },
	  { Pong3Table::ST_BORDER, 1, true, true },
	  { Pong3Table::ST_BORDER, 2, true, true },
	  { Pong3Table::ST_BORDER, 3, true, true },
	  { Pong3Table::ST_BORDER, 4, true, true },
	  { Pong3Table::ST_BORDER, 5, true, true } };
	CreatePaths(path_info, sizeof(path_info) / sizeof(path_info[0]));

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	AnimatePaths* animate_path =
	  new AnimatePaths(AnimatePaths::AT_CYCLE, color_set_.GetColor(0), 1,
			   table_,
			   table_->GetBaseLayer());
	for (int i = 0; i < num_paths_; ++i) {
	  animate_path->AddPath(paths_[i]);
	}
	animations_[anim_num++] = animate_path;
      }
      overhead_lights->SetColors(overhead_colors.GetColor(0));
      break;

    case 11: // Rotating triangles
      {
	ColorSet::ColorKeyframeInfo colors[] = {
	  { Color(30, 0, 0), 0.0 },  // Red green blue
	  { Color(0, 30, 0), 0.5 },
	  { Color(0, 0, 30), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // White light blue blue
	  { Color(0, 30, 30), 0.5 },
	  { Color(0, 0, 30), 1.0 },

	  { Color(30, 0, 0), 0.0 },  // Red white blue
	  { Color(30, 30, 30), 0.5 },
	  { Color(0, 0, 30), 1.0 },
	};
	const int colors_size = sizeof(colors) / sizeof(colors[0]);
	color_set_.AddColors(colors, colors_size);

	num_paths_ = 0;
	paths_ = NULL;

	num_animations_ = 1;
	animations_ = new Animation*[num_animations_];
	int anim_num = 0;
	
	animations_[anim_num++] = new RotatingTriangles(500,
							color_set_.GetColor(0),
							table_,
							table_->GetBaseLayer());
      }
      overhead_lights->SetColors(overhead_colors.GetColor(2));
      break;

    case 12: // Many Runners.
      {
	ColorSet::ColorKeyframeInfo trails[] = {
	  { Color(30, 30, 30), 0.0 },  // white to black
	  { Color(30, 30, 0), 0.1 },
	  { Color(20, 20, 20), 0.3 },
	  { Color(0, 0, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Red to Blue
	  { Color(30, 30, 0), 0.1 },
	  { Color(30, 0, 0), 0.3 },
	  { Color(0, 0, 10), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Blue to Green
	  { Color(30, 0, 30), 0.1 },
	  { Color(0, 0, 30), 0.3 },
	  { Color(0, 10, 0), 1.0 },

	  { Color(30, 30, 30), 0.0 },  // Green to Red
	  { Color(0, 20, 30), 0.1 },
	  { Color(0, 30, 0), 0.3 },
	  { Color(10, 0, 0), 1.0 },
	};
	const int trails_size = sizeof(trails) / sizeof(trails[0]);
	color_set_.AddColors(trails, trails_size);

	num_paths_ = 0;
	paths_ = NULL;

	num_animations_ = 48;
	animations_ = new Animation*[num_animations_];
	
	for (int i = 0; i < num_animations_; ++i) {
	  animations_[i] = new Runner(i * NUM_LEDS / num_animations_, 6,
				      color_set_.GetColor(i % color_set_.GetNumColors()),
				      table_,
				      table_->GetBaseLayer());
	}
      }
      overhead_lights->SetColors(overhead_colors.GetColor(2));
      break;

  };

  Serial.print("Available mem after starting pattern ");
  Serial.print(pattern_num);
  Serial.print(": ");
  Serial.println(availableMemory());
}

void TableController::UpdateSetting(int setting_num) {
  Configuration* config = &configuration_history_[current_config_];
  switch (config->pattern) {
    case 0: // Two player
    case 1: // Three player
    case 2: // Six player
    case 3: // Peace symbol
    case 4: // Jellyfish
    case 5: // Border cycle
    case 7: // Biohazard
    case 8: // Fire
    case 9: // Random static
    case 10: // Every segment
    case 11: // Rotating triangles
      config->setting = animations_[0]->UpdateSetting(setting_num);
      break;

    case 6: // Runners
      for (int i = 0; i < num_animations_; ++i) {
        config->setting = animations_[i]->UpdateSetting(setting_num);
      }
      break;
      
    default:
      config->setting = 0;
      break;
  }
}

void TableController::UpdateColorSet(int color_num) {
  Configuration* config = &configuration_history_[current_config_];
  switch (config->pattern) {
    case 0: // Two player
    case 1: // Three player
    case 2: // Six player
    case 3: // Peace symbol
    case 4: // Jellyfish
    case 5: // Border cycle
    case 7: // Biohazard
    case 8: // Fire
    case 9: // Random static
    case 10: // Every segment
    case 11: // Rotating triangles
      config->color = color_num % color_set_.GetNumColors();
      for (int i = 0; i < num_animations_; ++i) {
	animations_[i]->SetColors(color_set_.GetColor(config->color));
      }
      break;

    case 6: // Runners
      config->color = color_num % 3;
      if (config->color < 2) {
	// Three colors for 6 runners.
	for (int i = 0; i < num_animations_; ++i) {
	  animations_[i]->SetColors(color_set_.GetColor(config->color * 3 +
							(i % 3)));
	}
      } else {
	// Each runner gets its own color of the rainbow.
	for (int i = 0; i < num_animations_; ++i) {
	  animations_[i]->SetColors(color_set_.GetColor((i % 6) + 6));
	}
      }
      break;

    default:
      config->color = 0;
      break;
  }
}

void TableController::PushConfiguration() {
  ++current_config_;
  if (current_config_ >= MAX_HISTORY) {
    memmove(configuration_history_, &configuration_history_[1],
	    sizeof(Configuration) * (MAX_HISTORY - 1));
    current_config_ = MAX_HISTORY - 1;
  } else {
    Configuration* config = &configuration_history_[current_config_];
    if (current_config_ > 0) {
      memcpy(config, &configuration_history_[current_config_ - 1],
	     sizeof(Configuration));
    }
  }
}

void TableController::PopIfUnchanged() {
  if (current_config_ <= 0) return;

  if (configuration_history_[current_config_] ==
      configuration_history_[current_config_ - 1]) {
    --current_config_;
  }
}
