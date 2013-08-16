#include "AnimatePaths.h"

#include "ColorInterpolator.h"
#include "LedLayer.h"
#include "LedPath.h"
#include "Util.h"

AnimatePaths::AnimatePaths(AnimationType anim_type, ColorInterpolator* colors,
			   int speed,
			   Pong3Table* table, LedLayer* layer)
  : Animation(table, layer),
    anim_type_(anim_type),
    colors_(colors),
    paths_(NULL),
    num_paths_(0),
    step_(0),
    speed_(speed) {
}

AnimatePaths::~AnimatePaths() {
  free(paths_);
}

void AnimatePaths::Step() {
  int cycle_length = 64;
  if (anim_type_ != AT_FULL_COLOR_CYCLE) {
    ComputeCycleLength calc_cycle_len;
    for (int i = 0; i < num_paths_; ++i) {
      LedPath* path = paths_[i];
      calc_cycle_len.AddLength(path->GetLength());
    }
    cycle_length = calc_cycle_len.GetCycleLength();
  }

  step_ = (step_ + speed_) % cycle_length;

  switch (anim_type_) {
    /*
    case AT_WINDOW:
      {
	const float window_size_ = 0.5;
	// TODO: If anim type becomes a class, num_steps and window_size should
	// be settings on that class.
	int num_steps = 0;
	for (int i = 0; i < num_paths_; ++i) {
	  LedPath* path = paths_[i];
	  if (path->GetLength() > num_steps) {
	    num_steps = path->GetLength();
	  }
	}

	for (int i = 0; i < num_paths_; ++i) {
	  LedPath* path = paths_[i];
	  int interpolator_range = path->GetLength() / window_size_;
	  int window_range = interpolator_range - path->GetLength();
	  int cycle_step = ((step_ / num_steps) % 2 == 0) ?
	    (step_ % num_steps) :
	    (num_steps - 1 - (step_ % num_steps));

	  int window_start = window_range * cycle_step / (num_steps - 1);
	  int offset = 0;
	  for (LedPath::Iterator iter = path->begin();
	       iter != path->end();
	       ++iter, ++offset) {
	    int led = *iter;
	    int color_pos = window_start + offset;
	    layer_->SetColor(led, colors_->GetColor(color_pos,
						    interpolator_range));
	  }
	}
      }
      break;
*/
    case AT_CYCLE:
      for (int i = 0; i < num_paths_; ++i) {
	LedPath* path = paths_[i];
	int offset = 0;
	for (LedPath::Iterator iter = path->begin();
	     iter != path->end();
	     ++iter, ++offset) {
	  int led = *iter;
	  int color_offset = (path->GetLength() - offset + step_) % path->GetLength();
	  layer_->SetColor(led, colors_->GetColor(color_offset,
						  path->GetLength()));
	}
      }
      break;

    case AT_FULL_COLOR_CYCLE:
      {
	const int num_steps = 64;
	Color color = colors_->GetColor(step_ % num_steps, num_steps);
	for (int i = 0; i < num_paths_; ++i) {
	  LedPath* path = paths_[i];
	  int offset = 0;
	  for (LedPath::Iterator iter = path->begin();
	       iter != path->end();
	       ++iter, ++offset) {
	    int led = *iter;
	    layer_->SetColor(led, color);
	  }
	}
      }
      break;
  }
}

void AnimatePaths::AddPath(LedPath* path) {
  LedPath** old_alloc = paths_;
  paths_ = (LedPath**)realloc(paths_, sizeof(LedPath*) * (num_paths_ + 1));
  if (paths_ == NULL) {
    paths_ = old_alloc;
    return;
  }

  paths_[num_paths_] = path;
  ++num_paths_;
}

int AnimatePaths::UpdateSetting(int new_setting) {
  const int num_settings = NUM_ANIMATIONS * 2;

  setting_ = (new_setting + num_settings) % num_settings;
  speed_ = (setting_ % 2) * 2 + 1;
  anim_type_ = (AnimationType)((setting_ / 2) + AT_CYCLE);

  return setting_;
}

