//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_GAME_GAMEMAP_H_
#define U7_GAME_GAMEMAP_H_

#include "algorithm/Matrix.h"
#include "maze/Maze.h"

#include <memory>

namespace u7::game {

class GameMap {
 public:
  struct Location {
    int x = 0;
    int y = 0;

    [[nodiscard]] Location Up() const { return Location{x, y + 1}; };
    [[nodiscard]] Location Down() const { return Location{x, y - 1}; };
    [[nodiscard]] Location Left() const { return Location{x - 1, y}; };
    [[nodiscard]] Location Right() const { return Location{x + 1, y}; };

    bool operator==(const Location& rhs) const {
      return x == rhs.x && y == rhs.y;
    }
  };

  GameMap() = default;

  GameMap(maze::Maze maze, Location entrance, Location exit);

  [[nodiscard]] int GetWidth() const { return maze_.m(); }

  [[nodiscard]] int GetHeight() const { return maze_.n(); }

  [[nodiscard]] bool Contains(Location loc) const {
    return (loc.x >= 0 && loc.x < maze_.m() && loc.y >= 0 && loc.y < maze_.n());
  }

  [[nodiscard]] bool IsHall(Location loc) const {
    return Contains(loc) && maze_.UnsafeAt(loc.y, loc.x);
  }

  [[nodiscard]] bool IsWall(Location loc) const {
    return !Contains(loc) || !maze_.UnsafeAt(loc.y, loc.x);
  }

  [[nodiscard]] Location GetEntranceLocation() const { return entrance_; }

  [[nodiscard]] Location GetExitLocation() const { return exit_; }

  [[nodiscard]] size_t GetDistanceToExit(Location loc) const {
    return (Contains(loc) ? distanceToExit_.UnsafeAt(loc.y, loc.x)
                          : static_cast<size_t>(-1));
  }

  [[nodiscard]] size_t MaxDistanceToExit() const { return maxDistanceToExit_; }

 private:
  void InitDistanceToExit();

  maze::Maze maze_;
  Location entrance_;
  Location exit_;

  algorithm::Matrix<size_t> distanceToExit_;
  size_t maxDistanceToExit_ = 0;
};

std::shared_ptr<GameMap> GenGameMap(int width, int height, maze::Rng rng,
                                    maze::GenMazeOptions options = {});

}  // namespace u7::game

#endif  // U7_GAME_GAMEMAP_H_
