#include "game/SceneView.h"

#include <algorithm>
#include <iostream>

namespace u7::game {

void SceneView::ProcessPointOfInterest(SceneCoord coord) {
  const int innerScreenWidth = std::max(0, screenWidth_ - kInnerScreenMargin);
  const int innerScreenHeight = std::max(0, screenHeight_ - kInnerScreenMargin);
  const double innerSceneLeft =
      sceneViewCentre_.x - innerScreenWidth * screenScale_ / 2.0;
  const double innerSceneRight =
      sceneViewCentre_.x + innerScreenWidth * screenScale_ / 2.0;
  const double innerSceneTop =
      sceneViewCentre_.y + innerScreenHeight * screenScale_ / 2.0;
  const double innerSceneBottom =
      sceneViewCentre_.y - innerScreenHeight * screenScale_ / 2.0;
  const double cx = std::clamp(coord.x, innerSceneLeft, innerSceneRight);
  const double cy = std::clamp(coord.y, innerSceneBottom, innerSceneTop);
  sceneViewCentre_.x += coord.x - cx;
  sceneViewCentre_.y += coord.y - cy;
}

}  // namespace u7::game
