//
// Created by Alexander G. Pronchenkov on 28.01.2023.
//
#ifndef U7_GAME_SCENE_VIEW_H_
#define U7_GAME_SCENE_VIEW_H_

namespace u7::game {

// Coordinates on the scene.
//
// The scene's coordinate system is the same as the game location.
struct SceneCoord {
  double x = 0;
  double y = 0;
};

class SceneView {
 public:
  static constexpr int kInnerScreenMargin = 256;

  void ReshapeScreen(int screenWidth, int screenHeight) {
    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;
  }

  [[nodiscard]] double GetScreenScale() const { return screenScale_; }

  [[nodiscard]] int GetScreenWidth() const { return screenWidth_; }

  [[nodiscard]] int GetScreenHeight() const { return screenHeight_; }

  [[nodiscard]] SceneCoord GetBottomLeft() const {
    return {sceneViewCentre_.x - screenWidth_ * screenScale_ / 2,
            sceneViewCentre_.y - screenHeight_ * screenScale_ / 2};
  }

  [[nodiscard]] SceneCoord GetTopRight() const {
    return {sceneViewCentre_.x + screenWidth_ * screenScale_ / 2,
            sceneViewCentre_.y + screenHeight_ * screenScale_ / 2};
  }

  void ZoomIn();

  void ZoomOut();

  void SetSceneViewCentre(SceneCoord coord) { sceneViewCentre_ = coord; }

  void ProcessPointOfInterest(SceneCoord coord);

  void ProcessPointOfInterest(double sceneX, double sceneY) {
    ProcessPointOfInterest(SceneCoord{sceneX, sceneY});
  }

 private:
  double screenScale_ = 1 / 128.0;
  int screenWidth_ = 320;
  int screenHeight_ = 200;
  SceneCoord sceneViewCentre_ = {0, 0};
};

}  // namespace u7::game

#endif  // U7_GAME_SCENE_VIEW_H_
