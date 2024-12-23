//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/Game.h"
#include "game/Glyph.h"
#include "game/SceneView.h"
#include "palettes/Palettes.h"

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <random>
#include <span>

using ::u7::game::Game;
using ::u7::game::GameMap;
using ::u7::game::GenGameMap;
using ::u7::game::GetStandardGlyph;
using ::u7::game::Glyph;
using ::u7::game::SceneCoord;
using ::u7::game::SceneView;
using ::u7::maze::GenMazeOptions;
using ::u7::palettes::Colour3f;
using ::u7::palettes::GetColour;
using ::u7::palettes::GetCubehelixPalette;
using ::u7::palettes::GetHeatmap5Palette;

constexpr GenMazeOptions kGenMazeOptions{
    .noLoops = false,
    .noSmallSquares = false,
    .limitDensityR = 5,
    .limitDensityThreshold = 20,
    .pruneStubs = true,
};

constexpr int kScoreFontSize = 8;

void DrawGameMap(const GameMap& map, std::span<const Colour3f> palette,
                 Colour3f exitColour) {
  glBegin(GL_LINES);
  const auto colour = [&](GameMap::Location loc) {
    return GetColour(1.0f - (0.0625f + map.GetDistanceToExit(loc)) /
                                (0.0625f + map.MaxDistanceToExit()),
                     palette);
  };
  for (int x = 0; x < map.GetWidth(); ++x) {
    for (int y = 0; y < map.GetHeight(); ++y) {
      const GameMap::Location loc{x, y};
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

void DrawSquare() {
  glVertex2i(-1, -1);
  glVertex2i(+1, -1);
  glVertex2i(+1, +1);
  glVertex2i(-1, +1);
}

template <int N, int Dir>
void DrawCircle() {
  constexpr float kPi = 3.1415926535897f;
  const auto alpha = -2 * kPi / N;
  float area = 0.0;
  {
    float x0 = cosf(alpha * (N - 1));
    float y0 = sinf(alpha * (N - 1));
    for (int i = 0; i < N; ++i) {
      float x1 = cosf(alpha * i);
      float y1 = sinf(alpha * i);
      area += (x1 * y0 - y1 * x0) / 2;
      x0 = x1;
      y0 = y1;
    }
  }
  const float alpha0 = fmodf(glfwGetTime(), 2 * kPi);
  const auto radius = sqrtf(4 / area);
  for (int i = 0; i < N; ++i) {
    glVertex2f(radius * cosf(Dir * alpha0 + alpha * i),
               radius * sinf(Dir * alpha0 + alpha * i));
  }
}

void DrawGamePlayer(auto drawPointsFn, const Game::PlayerState& playerState,
                    Colour3f colour, float z) {
  glColor3f(colour.r, colour.g, colour.b);
  const auto& loc = playerState.location;
  glPushMatrix();
  {
    glTranslatef(loc.x, loc.y, z);
    {
      glPushMatrix();
      glScalef(0.5f, 0.5f, 1.0f);
      glBegin(GL_TRIANGLE_FAN);
      drawPointsFn();
      glEnd();
      glPopMatrix();
    }
    if (playerState.touchedExit) {
      glPushMatrix();
      glScalef(0.7f, 0.7f, 1.0f);
      glBegin(GL_LINE_LOOP);
      drawPointsFn();
      glEnd();
      glPopMatrix();
    }
    glPopMatrix();
  }
}

void Print(std::string_view message) {
  for (char ch : message) {
    glBegin(GL_QUADS);
    const auto& glyph = GetStandardGlyph(std::string_view(&ch, 1));
    for (const auto& [x, y] : glyph.pxls) {
      glVertex3i(x, y, 0);
      glVertex3i(x + 1, y, 0);
      glVertex3i(x + 1, y + 1, 0);
      glVertex3i(x, y + 1, 0);
    }
    glEnd();
    glTranslatef(glyph.dimension.x - glyph.base.x, 0.0f, 0.0f);
  }
}

int globalGameScore;
std::shared_ptr<Game> globalGame1;
std::shared_ptr<Game> globalGame2;
double globalLastGameActionTimePointSeconds;

enum Palette { DEFAULT, CUBEHELIX, HEATMAP };
GLuint globalSceneDisplayLists;

SceneView globalSceneView;

void MakeNewMap() {
  static std::mt19937 rng;
  const auto screenScale = globalSceneView.GetScreenScale();
  const auto screenWidth = globalSceneView.GetScreenWidth();
  const auto screenHeight = globalSceneView.GetScreenHeight();
  const int width = std::max<int>(
      (screenWidth - SceneView::kInnerScreenMargin) * screenScale, 3);
  const int height = std::max<int>(
      (screenHeight - SceneView::kInnerScreenMargin) * screenScale, 3);
  auto gameMap =
      GenGameMap(width, height, [&] { return rng(); }, kGenMazeOptions);
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
  globalSceneView.SetSceneViewCentre(SceneCoord{
      (gameMap->GetWidth() - 1) / 2.0, (gameMap->GetHeight() - 1) / 2.0});
  globalSceneView.ProcessPointOfInterest(gameMap->GetEntranceLocation().x,
                                         gameMap->GetEntranceLocation().y);
  globalLastGameActionTimePointSeconds = glfwGetTime();
}

void FramebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
  globalSceneView.ReshapeScreen(width, height);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action,
                 int /*mods*/) {
  if (action != GLFW_PRESS && action != GLFW_REPEAT) {
    return;
  }
  const auto player1Loc = globalGame1->GetPlayerState().location;
  const auto player2Loc = globalGame2->GetPlayerState().location;
  switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;

    case GLFW_KEY_R:
      MakeNewMap();
      return;

    case GLFW_KEY_MINUS:
      globalSceneView.ZoomOut();
      return;

    case GLFW_KEY_EQUAL:
      globalSceneView.ZoomIn();
      globalSceneView.ProcessPointOfInterest(player2Loc.x, player2Loc.y);
      globalSceneView.ProcessPointOfInterest(player1Loc.x, player1Loc.y);
      return;

    default:
      return;
  }
}

void KeyH(GLFWwindow* window) {
  const auto readPlayerActions = [&](int keyUp, int keyDown, int keyLeft,
                                     int keyRight, int keyAsk1, int keyAsk2,
                                     int gamepadId) {
    Game::PlayerActions result;
    {  // Keyboard
      if (glfwGetKey(window, keyUp) == GLFW_PRESS) {
        result |= Game::kPlayerGoUp;
      }
      if (glfwGetKey(window, keyDown) == GLFW_PRESS) {
        result |= Game::kPlayerGoDown;
      }
      if (glfwGetKey(window, keyLeft) == GLFW_PRESS) {
        result |= Game::kPlayerGoLeft;
      }
      if (glfwGetKey(window, keyRight) == GLFW_PRESS) {
        result |= Game::kPlayerGoRight;
      }
      if (glfwGetKey(window, keyAsk1) == GLFW_PRESS) {
        result |= Game::kPlayerAsk1;
      }
      if (glfwGetKey(window, keyAsk2) == GLFW_PRESS) {
        result |= Game::kPlayerAsk2;
      }
    }
    // Gamepad
    if (GLFWgamepadstate gamepadState;
        glfwGetGamepadState(gamepadId, &gamepadState)) {
      if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS) {
        result |= Game::kPlayerGoUp;
      }
      if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS) {
        result |= Game::kPlayerGoDown;
      }
      if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS) {
        result |= Game::kPlayerGoLeft;
      }
      if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS) {
        result |= Game::kPlayerGoRight;
      }
      if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS) {
        result |= Game::kPlayerAsk1;
      }
      if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] ==
          GLFW_PRESS) {
        result |= Game::kPlayerAsk2;
      }
      auto handleXYAxis = [&](float x, float y) {
        constexpr float kThreshold = 0.33;
        if (y < -kThreshold) {
          result |= Game::kPlayerGoUp;
        }
        if (y > kThreshold) {
          result |= Game::kPlayerGoDown;
        }
        if (x < -kThreshold) {
          result |= Game::kPlayerGoLeft;
        }
        if (x > kThreshold) {
          result |= Game::kPlayerGoRight;
        }
      };
      auto handleTriggerAxis = [&](float l, float r) {
        constexpr float kThreshold = 0.1;
        if (l > kThreshold) {
          result |= Game::kPlayerAsk1;
        }
        if (r > kThreshold) {
          result |= Game::kPlayerAsk2;
        }
      };
      handleXYAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X],
                   gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
      handleXYAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
                   gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
      handleTriggerAxis(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER],
                        gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);
    }
    return result;
  };

  const auto player1Actions = readPlayerActions(                  //
      GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT,  //
      GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT,                  //
      GLFW_JOYSTICK_1);
  const auto player2Actions = readPlayerActions(       //
      GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D,  //
      GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT,       //
      GLFW_JOYSTICK_2);

  const auto now = glfwGetTime();
  const auto secondsElapse =
      (now - std::exchange(globalLastGameActionTimePointSeconds, now));

  const auto game1 = globalGame1;
  const auto game2 = globalGame2;
  game1->ApplyPlayerActions(player1Actions, secondsElapse);
  game2->ApplyPlayerActions(player2Actions, secondsElapse);
  const auto player1Loc = game1->GetPlayerState().location;
  const auto player2Loc = game2->GetPlayerState().location;
  globalSceneView.ProcessPointOfInterest(player1Loc.x, player1Loc.y);
  globalSceneView.ProcessPointOfInterest(player2Loc.x, player2Loc.y);
}

void Draw() {
  const auto player1State = globalGame1->GetPlayerState();
  const auto player2State = globalGame2->GetPlayerState();
  const auto bottomLeft = globalSceneView.GetBottomLeft();
  const auto topRight = globalSceneView.GetTopRight();
  const auto ask1 = (player1State.ask1 || player2State.ask1);
  const auto ask2 = (player1State.ask2 || player2State.ask2);
  auto palette = Palette::DEFAULT;
  if (ask1 && !ask2) {
    palette = Palette::CUBEHELIX;
  } else if (!ask1 && ask2) {
    palette = Palette::HEATMAP;
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_FLAT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glOrtho(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, -5.0, 5.0);
  glCallList(globalSceneDisplayLists + palette);
  DrawGamePlayer(&DrawCircle<5, 1>, player1State, Colour3f{0.94f, 0.72f, 0.82f},
                 3.0f);
  DrawGamePlayer(&DrawCircle<5, -1>, player2State,
                 Colour3f{0.91f, 0.34f, 0.57f}, 2.0f);
  {
    const auto sp = GetStandardGlyph(" ");
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0.0, globalSceneView.GetScreenWidth(), 0.0,
            globalSceneView.GetScreenHeight(), -5.0, 5.0);
    glTranslatef(0, globalSceneView.GetScreenHeight(), 0.0);
    glTranslatef(30.0, -30.0, 4.0);
    glScalef(kScoreFontSize, kScoreFontSize, 1.0f);
    glTranslatef(sp.dimension.x - sp.base.x, -sp.dimension.y + sp.base.y, 0.0);
    glColor3f(255 / 255.0f, 235 / 255.0f, 185 / 255.0f);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d", 10 * globalGameScore);
    Print(buf);
  }
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
  glfwSwapInterval(1);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glLineWidth(2.0f);

  globalSceneDisplayLists = glGenLists(3);

  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    FramebufferSizeCallback(window, width, height);
  }

  MakeNewMap();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    KeyH(window);
    {
      auto game1 = globalGame1;
      auto game2 = globalGame2;
      auto player1State = game1->GetPlayerState();
      auto player2State = game2->GetPlayerState();
      if ((player1State.touchedExit && player2State.touchedExit) ||
          (std::fabs(player1State.location.x - player2State.location.x) < 0.5 &&
           std::fabs(player1State.location.y - player2State.location.y) < 0.5 &&
           (player1State.touchedExit || player2State.touchedExit))) {
        globalGameScore += 1;
        MakeNewMap();
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
