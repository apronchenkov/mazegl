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

    Location Up() const { return Location{x, y + 1}; };
    Location Down() const { return Location{x, y - 1}; };
    Location Left() const { return Location{x - 1, y}; };
    Location Right() const { return Location{x + 1, y}; };

    bool operator==(const Location& rhs) const {
      return x == rhs.x && y == rhs.y;
    }
  };

  GameMap() = default;

  GameMap(maze::Maze maze, Location entrance, Location exit);

  int GetWidth() const { return maze_.m(); }

  int GetHeight() const { return maze_.n(); }

  bool Contains(Location loc) const {
    return (loc.x >= 0 && loc.x < maze_.m() && loc.y >= 0 && loc.y < maze_.n());
  }

  bool IsHall(Location loc) const {
    return Contains(loc) && maze_.UnsafeAt(loc.y, loc.x);
  }

  bool IsWall(Location loc) const {
    return !Contains(loc) || !maze_.UnsafeAt(loc.y, loc.x);
  }

  Location GetEntranceLocation() const { return entrance_; }

  Location GetExitLocation() const { return exit_; }

  size_t GetDistanceToExit(Location loc) const {
    if (Contains(loc)) {
      return distanceToExit_.UnsafeAt(loc.y, loc.x);
    }
    return static_cast<size_t>(-1);
  }

  size_t MaxDistanceToExit() const { return maxDistanceToExit_; }

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
