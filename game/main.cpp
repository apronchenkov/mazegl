//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/Game.h"
#include "game/KeyStateMachine.h"
#include "palettes/Palettes.h"

#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <random>
#include <span>

using ::u7::game::Game;
using ::u7::game::GameLocation;
using ::u7::game::GameMap;
using ::u7::game::GameState;
using ::u7::game::GenGameMap;
using ::u7::game::KeyStateMachine;
using ::u7::game::PlayerAction;
using ::u7::maze::GenMazeOptions;
using ::u7::palettes::Colour3f;
using ::u7::palettes::GetColour;
using ::u7::palettes::GetCubehelixPalette;
using ::u7::palettes::GetHeatmap5Palette;

constexpr GenMazeOptions kGenMazeOptions{
    // .noLoops = false,
    // .noSmallSquares = false,
    // .limitDensityR = 3,
    // .limitDensityThreshold = 7,
    // .pruneStubs = false,
};

constexpr int kGameCellSize = 32;

void DrawGameMap(const GameMap& map, std::span<const Colour3f> palette,
                 Colour3f exitColour) {
  glBegin(GL_LINES);
  const auto colour = [&](GameLocation loc) {
    return GetColour(1.0f - (0.0625f + map.GetDistanceToExit(loc)) /
                                (0.0625f + map.MaxDistanceToExit()),
                     palette);
  };
  for (int x = 0; x < map.GetWidth(); ++x) {
    for (int y = 0; y < map.GetHeight(); ++y) {
      const GameLocation loc{x, y};
      if (map.IsHall(loc)) {
        const auto locColour = colour(loc);
        for (auto newLoc : {loc.Right(), loc.Up()}) {
          if (map.IsHall(newLoc)) {
            const auto newLocColour = colour(newLoc);
            glColor3f(locColour.r, locColour.g, locColour.b);
            glVertex2i(loc.x, loc.y);
            glColor3f(newLocColour.r, newLocColour.g, newLocColour.b);
            glVertex2i(newLoc.x, newLoc.y);
          }
        }
      }
    }
  }
  glEnd();
  {
    const auto loc = map.GetExitLocation();
    glBegin(GL_QUADS);
    glColor3f(exitColour.r, exitColour.g, exitColour.b);
    glVertex3f(loc.x - 0.5f, loc.y - 0.5f, 1.0f);
    glVertex3f(loc.x + 0.5f, loc.y - 0.5f, 1.0f);
    glVertex3f(loc.x + 0.5f, loc.y + 0.5f, 1.0f);
    glVertex3f(loc.x - 0.5f, loc.y + 0.5f, 1.0f);
    glEnd();
    glColor3f(exitColour.r, exitColour.g, exitColour.b);
    glBegin(GL_LINE_LOOP);
    glVertex3f(loc.x - 0.7f, loc.y - 0.7f, 1.0f);
    glVertex3f(loc.x + 0.7f, loc.y - 0.7f, 1.0f);
    glVertex3f(loc.x + 0.7f, loc.y + 0.7f, 1.0f);
    glVertex3f(loc.x - 0.7f, loc.y + 0.7f, 1.0f);
    glEnd();
  }
}

void DrawGamePlayer(const Game& game, Colour3f colour, float z) {
  const auto loc = game.GetPlayerLocation();
  glColor3f(colour.r, colour.g, colour.b);
  glBegin(GL_QUADS);
  glVertex3f(loc.x - 0.5f, loc.y - 0.5f, z);
  glVertex3f(loc.x + 0.5f, loc.y - 0.5f, z);
  glVertex3f(loc.x + 0.5f, loc.y + 0.5f, z);
  glVertex3f(loc.x - 0.5f, loc.y + 0.5f, z);
  glEnd();
  if (game.GetGameState() == GameState::COMPLETE) {
    glBegin(GL_LINE_LOOP);
    glVertex3f(loc.x - 0.7f, loc.y - 0.7f, z);
    glVertex3f(loc.x + 0.7f, loc.y - 0.7f, z);
    glVertex3f(loc.x + 0.7f, loc.y + 0.7f, z);
    glVertex3f(loc.x - 0.7f, loc.y + 0.7f, z);
    glEnd();
  }
}

int globalGameMapHeight;
int globalGameMapWidth;

std::shared_ptr<Game> globalGame1;
std::shared_ptr<Game> globalGame2;

enum Palette { DEFAULT, CUBEHELIX, HEATMAP, LAST = HEATMAP };
Palette globalPalette = Palette::DEFAULT;
GLuint globalSceneDisplayLists;

void MakeNewMap() {
  static std::mt19937 rng;
  auto gameMap = GenGameMap(
      globalGameMapWidth, globalGameMapHeight, [&] { return rng(); },
      kGenMazeOptions);

  {
    static const auto defaultPalette = {
        Colour3f{147 / 255.0f, 147 / 255.0f, 147 / 255.0f}};
    static const auto defaultExitColour =
        Colour3f{252 / 255.0f, 246 / 255.0f, 182 / 255.0f};
    glNewList(globalSceneDisplayLists + Palette::DEFAULT, GL_COMPILE);
    DrawGameMap(*gameMap, defaultPalette, defaultExitColour);
    glEndList();

    static const auto cubehelixPalette =
        GetCubehelixPalette(256, /*begin=*/0.1, /*end=*/0.95);
    static const auto cubehelixExitColour = GetColour(1.0f, cubehelixPalette);
    glNewList(globalSceneDisplayLists + Palette::CUBEHELIX, GL_COMPILE);
    DrawGameMap(*gameMap, cubehelixPalette, cubehelixExitColour);
    glEndList();

    static const auto heatmapPalette = GetHeatmap5Palette();
    static const auto heatmapExitColour = GetColour(1.0f, heatmapPalette);
    glNewList(globalSceneDisplayLists + Palette::HEATMAP, GL_COMPILE);
    DrawGameMap(*gameMap, heatmapPalette, heatmapExitColour);
    glEndList();
  }
  globalGame1 = std::make_shared<Game>(gameMap);
  globalGame2 = std::make_shared<Game>(gameMap);
}

struct SceneLocation {
  double x = 0;
  double y = 0;
};

class SceneView {
 public:
  void Reshape(double width, double height) {
    width_ = width;
    height_ = height;
    Recalculate();
  }
  void UpdateCursor(SceneLocation loc) {
    cursorX_ = loc.x;
    cursorY_ = loc.y;
    Recalculate();
  }

  double GetLeft() const { return x0_ * scale_; }
  double GetRight() const { return (x0_ + width_) * scale_; }
  double GetBottom() const { return y0_ * scale_; }
  double GetTop() const { return (y0_ + height_) * scale_; };

  SceneLocation ToSceneLocation(GameLocation loc) const {
    return {loc.x / scale_, loc.y / scale_};
  }

 private:
  void Recalculate() {
    constexpr double outerGap = 400;
    const double innerWidth = std::max(0.0, width_ - outerGap);
    const double innerHeight = std::max(0.0, height_ - outerGap);
    const double innerLeft = x0_ + width_ / 2 - innerWidth / 2;
    const double innerRight = x0_ + width_ / 2 + innerWidth / 2;
    const double innerBottom = y0_ + height_ / 2 - innerHeight / 2;
    const double innerTop = y0_ + height_ / 2 + innerHeight / 2;
    const double cx = std::clamp(cursorX_, innerLeft, innerRight);
    const double cy = std::clamp(cursorY_, innerBottom, innerTop);
    x0_ -= cx - cursorX_;
    y0_ -= cy - cursorY_;
  }

  double scale_ = 1.0 / kGameCellSize;
  double x0_ = 0;
  double y0_ = 0;
  double width_ = 320.0;
  double height_ = 200.0;
  double cursorX_ = 0.0;
  double cursorY_ = 0.0;
};

SceneView globalSceneView;

void FramebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
  globalSceneView.Reshape(width, height);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

KeyStateMachine globalKeyState1;
KeyStateMachine globalKeyState2;

void UpdateKeysetStateMachines(GLFWwindow* window) {
  const auto impl = [&](std::chrono::steady_clock::time_point now, int keyUp,
                        int keyDown, int keyLeft, int keyRight,
                        KeyStateMachine* keyState) {
    KeyStateMachine::Keyset keys;
    keys.set(KeyStateMachine::UP, glfwGetKey(window, keyUp) == GLFW_PRESS);
    keys.set(KeyStateMachine::DOWN, glfwGetKey(window, keyDown) == GLFW_PRESS);
    keys.set(KeyStateMachine::LEFT, glfwGetKey(window, keyLeft) == GLFW_PRESS);
    keys.set(KeyStateMachine::RIGHT,
             glfwGetKey(window, keyRight) == GLFW_PRESS);
    keyState->Update(now, keys);
  };
  const auto now = std::chrono::steady_clock::now();
  impl(now, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, &globalKeyState1);
  impl(now, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT,
       &globalKeyState2);
}

void KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action,
                 int /*mods*/) {
  UpdateKeysetStateMachines(window);
  if (action != GLFW_PRESS && action != GLFW_REPEAT) {
    return;
  }
  switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;

    case GLFW_KEY_R:
      MakeNewMap();
      globalSceneView.UpdateCursor(
          globalSceneView.ToSceneLocation(globalGame1->GetPlayerLocation()));
      globalSceneView.UpdateCursor(
          globalSceneView.ToSceneLocation(globalGame2->GetPlayerLocation()));
      return;

    default:
      return;
  }
}

void KeyH(GLFWwindow* window) {
  const auto playerAction = [&](KeyStateMachine::Keyset keyset, Game* game) {
    const auto loc = game->GetPlayerLocation();
    const auto& map = game->GetGameMap();
    const bool u = (keyset.test(KeyStateMachine::UP) && map.IsHall(loc.Up()));
    const bool d =
        (keyset.test(KeyStateMachine::DOWN) && map.IsHall(loc.Down()));
    const bool l =
        (keyset.test(KeyStateMachine::LEFT) && map.IsHall(loc.Left()));
    const bool r =
        (keyset.test(KeyStateMachine::RIGHT) && map.IsHall(loc.Right()));
    if (u + d + l + r != 1) {
      return;
    }
    if (u) {
      game->PerformPlayerAction(PlayerAction::GO_UP);
    } else if (d) {
      game->PerformPlayerAction(PlayerAction::GO_DOWN);
    } else if (l) {
      game->PerformPlayerAction(PlayerAction::GO_LEFT);
    } else if (r) {
      game->PerformPlayerAction(PlayerAction::GO_RIGHT);
    }
    globalSceneView.UpdateCursor(
        globalSceneView.ToSceneLocation(game->GetPlayerLocation()));
  };
  auto game1 = globalGame1;
  auto game2 = globalGame2;
  UpdateKeysetStateMachines(window);
  playerAction(globalKeyState1.Read(), game1.get());
  playerAction(globalKeyState2.Read(), game2.get());
  const auto lshift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
  const auto rshift = (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
  if (lshift && !rshift) {
    globalPalette = Palette::CUBEHELIX;
  } else if (rshift && !lshift) {
    globalPalette = Palette::HEATMAP;
  } else {
    globalPalette = Palette::DEFAULT;
  }
}

void Draw() {
  const auto game1 = globalGame1;
  const auto game2 = globalGame2;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_FLAT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(globalSceneView.GetLeft(), globalSceneView.GetRight(),
          globalSceneView.GetBottom(), globalSceneView.GetTop(), -5.0, 5.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glCallList(globalSceneDisplayLists + globalPalette);
  DrawGamePlayer(*game1, Colour3f{0.94f, 0.72f, 0.82f}, 3.0f);
  DrawGamePlayer(*game2, Colour3f{0.91f, 0.34f, 0.57f}, 2.0f);
}

int SubMain() {
  glfwWindowHint(GLFW_DEPTH_BITS, 16);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  GLFWwindow* window =
      glfwCreateWindow(mode->width, mode->height, "Maze", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to open GLFW window\n";
    return -1;
  }

  glfwSetWindowSizeLimits(window, 320, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);

  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetKeyCallback(window, KeyCallback);

  glfwMakeContextCurrent(window);
  //  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  globalSceneDisplayLists = glGenLists(3);

  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    FramebufferSizeCallback(window, width, height);
    globalGameMapWidth = std::max((width - 400) / kGameCellSize, 3);
    globalGameMapHeight = std::max((height - 400) / kGameCellSize, 3);
  }

  MakeNewMap();
  globalSceneView.UpdateCursor(
      globalSceneView.ToSceneLocation(globalGame1->GetPlayerLocation()));

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    KeyH(window);
    {
      auto game1 = globalGame1;
      auto game2 = globalGame2;
      if ((game1->GetGameState() == GameState::COMPLETE &&
           game2->GetGameState() == GameState::COMPLETE) ||
          (game1->GetPlayerLocation() == game2->GetPlayerLocation() &&
           (game1->GetGameState() == GameState::COMPLETE ||
            game2->GetGameState() == GameState::COMPLETE))) {
        MakeNewMap();
        globalSceneView.UpdateCursor(
            globalSceneView.ToSceneLocation(globalGame1->GetPlayerLocation()));
      }
    }
    Draw();
    glfwSwapBuffers(window);
  }
  return 0;
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }
  int result;
  try {
    result = SubMain();
  } catch (...) {
    glfwTerminate();
    throw;
  }
  glfwTerminate();
  return result;
}