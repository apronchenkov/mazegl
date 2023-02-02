//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/Game.h"

#include <cmath>
#include <iostream>

namespace u7::game {
namespace {

constexpr double kEps = 1.0 / 1024.0;

Game::PlayerState NormalizePlayerState(Game::PlayerState playerState,
                                       const GameMap& map) {
  const GameMap::Location mapLoc = {
      static_cast<int>(std::round(playerState.location.x)),
      static_cast<int>(std::round(playerState.location.y)),
  };
  if (!map.IsHall(mapLoc)) {
    throw std::logic_error("player has stuck in the wall");
  }
  if ((playerState.location.y > mapLoc.y && !map.IsHall(mapLoc.Up())) ||
      (playerState.location.y < mapLoc.y && !map.IsHall(mapLoc.Down()))) {
    playerState.location.y = mapLoc.y;
  }
  if ((playerState.location.x > mapLoc.x && !map.IsHall(mapLoc.Right())) ||
      (playerState.location.x < mapLoc.x && !map.IsHall(mapLoc.Left()))) {
    playerState.location.x = mapLoc.x;
  }
  if (std::fabs(playerState.location.x - mapLoc.x) < kEps &&
      std::fabs(playerState.location.y - mapLoc.y) < kEps &&
      map.GetExitLocation() == mapLoc) {
    playerState.touchedExit = true;
  }
  return playerState;
}

}  // namespace

bool Game::Location::IsCloseTo(const Location& rhs) const {
  return std::fabs(x - rhs.x) < kEps && std::fabs(y - rhs.y) < kEps;
}

Game::Game(std::shared_ptr<const GameMap> map) : map_(std::move(map)) {
  playerState_.location.x = map_->GetEntranceLocation().x;
  playerState_.location.y = map_->GetEntranceLocation().y;
  playerState_ = NormalizePlayerState(playerState_, *map_);
}

void Game::ApplyPlayerActions(PlayerActions actions, double seconds) {
  constexpr double kSpeed = 6.0;
  while (seconds > 0) {
    const GameMap::Location mapLoc = {
        static_cast<int>(std::round(playerState_.location.x)),
        static_cast<int>(std::round(playerState_.location.y)),
    };
    const Location deltaLoc = {
        playerState_.location.x - mapLoc.x,
        playerState_.location.y - mapLoc.y,
    };
    const double maxDelta = seconds * kSpeed;
    double deltaU = 0.0;
    double deltaD = 0.0;
    double deltaL = 0.0;
    double deltaR = 0.0;

    if (std::fabs(deltaLoc.x) < kEps && std::fabs(deltaLoc.y) < kEps) {
      if (map_->IsHall(mapLoc.Up())) {
        deltaU = 1.0 - deltaLoc.y;
      }
      if (map_->IsHall(mapLoc.Down())) {
        deltaD = 1.0 + deltaLoc.y;
      }
      if (map_->IsHall(mapLoc.Left())) {
        deltaL = 1.0 + deltaLoc.x;
      }
      if (map_->IsHall(mapLoc.Right())) {
        deltaR = 1.0 - deltaLoc.x;
      }
    } else {
      if (deltaLoc.y >= kEps) {
        deltaU = (map_->IsHall(mapLoc.Up()) ? 1.0 - deltaLoc.y : 0.0);
        deltaD = deltaLoc.y;
      } else if (deltaLoc.y <= -kEps) {
        deltaU = -deltaLoc.y;
        deltaD = (map_->IsHall(mapLoc.Down()) ? 1.0 + deltaLoc.y : 0.0);
      }
      if (deltaLoc.x >= kEps) {
        deltaL = deltaLoc.x;
        deltaR = (map_->IsHall(mapLoc.Right()) ? 1.0 - deltaLoc.x : 0.0);
      } else if (deltaLoc.x <= -kEps) {
        deltaL = (map_->IsHall(mapLoc.Left()) ? 1.0 + deltaLoc.x : 0.0);
        deltaR = -deltaLoc.x;
      }
    }
    const bool u = (actions.test(PlayerAction::GO_UP) && deltaU > 0);
    const bool d = (actions.test(PlayerAction::GO_DOWN) && deltaD > 0);
    const bool l = (actions.test(PlayerAction::GO_LEFT) && deltaL > 0);
    const bool r = (actions.test(PlayerAction::GO_RIGHT) && deltaR > 0);
    if (u + d + l + r != 1) {
      return;  // no aciton
    }
    if (u) {
      const double delta = std::min(maxDelta, deltaU);
      playerState_.location.x = mapLoc.x;
      playerState_.location.y += delta;
      seconds -= delta / kSpeed;
    }
    if (d) {
      const double delta = std::min(maxDelta, deltaD);
      playerState_.location.x = mapLoc.x;
      playerState_.location.y -= delta;
      seconds -= delta / kSpeed;
    }
    if (l) {
      const double delta = std::min(maxDelta, deltaL);
      playerState_.location.x -= delta;
      playerState_.location.y = mapLoc.y;
      seconds -= delta / kSpeed;
    }
    if (r) {
      const double delta = std::min(maxDelta, deltaR);
      playerState_.location.x += delta;
      playerState_.location.y = mapLoc.y;
      seconds -= delta / kSpeed;
    }
    playerState_ = NormalizePlayerState(playerState_, *map_);
  }
}

}  // namespace u7::game
