//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/GameMap.h"

#include <stdexcept>
#include <vector>

namespace u7::game {

using ::u7::algorithm::Matrix;

GameMap::GameMap(maze::Maze maze, Location entrance, Location exit)
    : maze_(std::move(maze)), entrance_(entrance), exit_(exit) {
  if (!Contains(entrance_)) {
    throw std::runtime_error("entrance location does not belong to the map");
  }
  if (!Contains(exit_)) {
    throw std::runtime_error("exit location does not belong to the map");
  }
  InitDistanceToExit();
  if (GetDistanceToExit(entrance) == static_cast<size_t>(-1)) {
    throw std::runtime_error("there is no path from entrance to exit");
  }
}

void GameMap::InitDistanceToExit() {
  std::vector<Location> frontier;
  std::vector<Location> nextFrontier;
  size_t distance = 0;
  distanceToExit_ = Matrix<size_t>(maze_.n(), maze_.m());
  distanceToExit_.Fill(static_cast<size_t>(-1));
  distanceToExit_.UnsafeAt(exit_.y, exit_.x) = distance;
  frontier.push_back(exit_);
  while (!frontier.empty()) {
    distance += 1;
    for (auto loc : frontier) {
      for (auto nextLoc : {loc.Down(), loc.Left(), loc.Right(), loc.Up()}) {
        if (IsHall(nextLoc) &&
            distanceToExit_.UnsafeAt(nextLoc.y, nextLoc.x) > distance) {
          distanceToExit_.UnsafeAt(nextLoc.y, nextLoc.x) = distance;
          nextFrontier.push_back(nextLoc);
        }
      }
    }
    std::swap(frontier, nextFrontier);
    nextFrontier.clear();
  }
  maxDistanceToExit_ = distance - 1;
}

}  // namespace u7::game
