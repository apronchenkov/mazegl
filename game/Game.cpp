//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/Game.h"

#include <cassert>
#include <vector>

namespace u7::game {

using ::u7::algorithm::Matrix;

GameMap::GameMap(maze::Maze maze, GameLocation entrance, GameLocation exit)
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
  std::vector<GameLocation> frontier;
  std::vector<GameLocation> nextFrontier;
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

std::shared_ptr<GameMap> GenGameMap(int width, int height, maze::Rng rng,
                                    maze::GenMazeOptions options) {
  auto maze = GenMaze(height, width, rng, options);
  GameLocation entrance;
  GameLocation exit;
  int entranceD = width + height;
  int exitD = 0;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (maze.UnsafeAt(y, x)) {
        const int d = x + y;
        if (entranceD > d) {
          entrance.x = x;
          entrance.y = y;
          entranceD = d;
        }
        if (exitD < d) {
          exit.x = x;
          exit.y = y;
          exitD = d;
        }
      }
    }
  }
  return std::make_shared<GameMap>(std::move(maze), entrance, exit);
}

Game::Game(std::shared_ptr<GameMap> map)
    : map_(std::move(map)),
      playerLocation_(map_->GetEntranceLocation()),
      gameState_(map_->GetEntranceLocation() == map_->GetExitLocation()
                     ? GameState::COMPLETE
                     : GameState::IN_PROGRESS) {}

void Game::PerformPlayerAction(PlayerAction action) {
  GameLocation newPlayerLocation;
  switch (action) {
    case PlayerAction::GO_UP:
      newPlayerLocation = playerLocation_.Up();
      break;
    case PlayerAction::GO_LEFT:
      newPlayerLocation = playerLocation_.Left();
      break;
    case PlayerAction::GO_RIGHT:
      newPlayerLocation = playerLocation_.Right();
      break;
    case PlayerAction::GO_DOWN:
      newPlayerLocation = playerLocation_.Down();
      break;
  }
  if (map_->IsHall(newPlayerLocation)) {
    playerLocation_ = newPlayerLocation;
    if (playerLocation_ == map_->GetExitLocation()) {
      gameState_ = GameState::COMPLETE;
    }
  }
}

}  // namespace u7::game
