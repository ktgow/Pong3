#include <Arduino.h>
#include "KeyFrameColorInterpolator.h"
#include "Util.h"

KeyFrameColorInterpolator::KeyFrameColorInterpolator()
  : frames_(NULL),
    lookup_(NULL),
    cache_enabled_(true) {
}

KeyFrameColorInterpolator::~KeyFrameColorInterpolator() {
  while (frames_ != NULL) {
    KeyFrameList* node = frames_;
    frames_ = frames_->next;
    delete node;
  }

  if (lookup_ != NULL) {
    delete[] lookup_;
    lookup_ = NULL;
  }
}

void KeyFrameColorInterpolator::AddKeyFrame(KeyFrame frame) {
  KeyFrameList* new_node = new KeyFrameList;
  new_node->frame = frame;
  new_node->next = NULL;

  // Find the frame we'll insert after.
  KeyFrameList* node = frames_;
  KeyFrameList* prev = NULL;
  while (node && frame.distance > node->frame.distance) {
    prev = node;
    node = node->next;
  }
  // Insert after prev.
  if (prev == NULL) {
    new_node->next = frames_;
    frames_ = new_node;
  } else {
    new_node->next = prev->next;
    prev->next = new_node;
  }

  if (lookup_ != NULL) {
    delete[] lookup_;
    lookup_ = NULL;
  }
}

Color KeyFrameColorInterpolator::GetColor(int step, int num_steps) {
  if (!cache_enabled_) {
    return GetColorInternal(step, num_steps);
  }

  // Generate lookup table if this is the first lookup since keys were added.
  if (lookup_ == NULL) {
    lookup_ = new Color[LOOKUP_SIZE];
    if (lookup_ == NULL) {
      return Color(0, 0, 0);
    }

    for (int i = 0; i < LOOKUP_SIZE; ++i) {
      lookup_[i] = GetColorInternal(i, LOOKUP_SIZE);
    }
  }

  int index = (step * (LOOKUP_SIZE - 1) / (num_steps - 1));
  if (index < 0) index = 0;
  else if (index >= LOOKUP_SIZE) index = LOOKUP_SIZE - 1;
  return lookup_[index];
}

Color KeyFrameColorInterpolator::GetColorInternal(int step, int num_steps) {
  // Find the nodes that this step lies between.
  long step_dist = (long)step * (long)KeyFrame::DIST_MAX / (long)(num_steps - 1);
  if (step_dist < 0) step_dist = 0;
  if (step_dist > KeyFrame::DIST_MAX) step_dist = KeyFrame::DIST_MAX;

  KeyFrameList* node = frames_;
  KeyFrameList* prev = node;  // Set to node so it isn't null if dist is 0.
  while (node && step_dist > node->frame.distance) {
    prev = node;
    node = node->next;
  }
  if (node == NULL) {
    node = prev;
  }

  // Interpolate between these two nodes.
  long relative_dist = step_dist - prev->frame.distance;
  long frame_dist = node->frame.distance - prev->frame.distance;
  if (frame_dist <= 0) {
    return prev->frame.color;
  }
  if (relative_dist < 0) relative_dist = 0;
  if (relative_dist > frame_dist) relative_dist = frame_dist;
  return interp(prev->frame.color, node->frame.color,
                relative_dist * 255 / frame_dist, 256);
}

void KeyFrameColorInterpolator::SetCacheEnabled(bool enabled) {
  if (cache_enabled_ == enabled) return;

  if (!enabled) {
    if (lookup_ != NULL) {
      delete[] lookup_;
      lookup_ = NULL;
    }
  }
  cache_enabled_ = enabled;
}
