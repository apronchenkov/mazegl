//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_GAME_GAME_H
#define U7_GAME_GAME_H

#pragma once
#include "algorithm/Matrix.h"
#include "maze/Maze.h"

#include <memory>

namespace u7::game {

struct GameLocation {
  int x = 0;
  int y = 0;

  GameLocation Up() const { return GameLocation{x, y + 1}; };
  GameLocation Down() const { return GameLocation{x, y - 1}; };
  GameLocation Left() const { return GameLocation{x - 1, y}; };
  GameLocation Right() const { return GameLocation{x + 1, y}; };

  bool operator==(const GameLocation& rhs) const {
    return x == rhs.x && y == rhs.y;
  }
};

class GameMap {
 public:
  GameMap() = default;

  GameMap(maze::Maze maze, GameLocation entrance, GameLocation exit);

  int GetWidth() const { return maze_.m(); }

  int GetHeight() const { return maze_.n(); }

  bool Contains(GameLocation loc) const {
    return (loc.x >= 0 && loc.x < maze_.m() && loc.y >= 0 && loc.y < maze_.n());
  }

  bool IsHall(GameLocation loc) const {
    return Contains(loc) && maze_.UnsafeAt(loc.y, loc.x);
  }

  bool IsWall(GameLocation loc) const {
    return !Contains(loc) || !maze_.UnsafeAt(loc.y, loc.x);
  }

  GameLocation GetEntranceLocation() const { return entrance_; }

  GameLocation GetExitLocation() const { return exit_; }

  size_t GetDistanceToExit(GameLocation loc) const {
    if (Contains(loc)) {
      return distanceToExit_.UnsafeAt(loc.y, loc.x);
    }
    return static_cast<size_t>(-1);
  }

  size_t MaxDistanceToExit() const { return maxDistanceToExit_; }

 private:
  void InitDistanceToExit();

  maze::Maze maze_;
  GameLocation entrance_;
  GameLocation exit_;

  algorithm::Matrix<size_t> distanceToExit_;
  size_t maxDistanceToExit_ = 0;
};

std::shared_ptr<GameMap> GenGameMap(int width, int height, maze::Rng rng,
                                    maze::GenMazeOptions options = {});

enum class GameState {
  IN_PROGRESS,
  COMPLETE,
};

enum class PlayerAction {
  GO_UP,
  GO_LEFT,
  GO_RIGHT,
  GO_DOWN,
};

class Game {
 public:
  explicit Game(std::shared_ptr<GameMap> map);

  const GameMap& GetGameMap() const { return *map_; }

  GameState GetGameState() const { return gameState_; }

  GameLocation GetPlayerLocation() const { return playerLocation_; }

  void PerformPlayerAction(PlayerAction action);

 private:
  std::shared_ptr<GameMap> map_;
  GameLocation playerLocation_;
  GameState gameState_ = GameState::IN_PROGRESS;
};

}  // namespace u7::game

#endif  // U7_GAME_GAME_H
