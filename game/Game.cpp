//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/Game.h"

#include <cmath>

namespace u7::game {
namespace {

constexpr double kEps = 1.0 / 1024.0;
constexpr double kBaseSpeed = 2.0;
constexpr double kAcceleration = 15.0;

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

Game::Game(std::shared_ptr<const GameMap> map) : map_(std::move(map)) {
  playerState_.location.x = map_->GetEntranceLocation().x;
  playerState_.location.y = map_->GetEntranceLocation().y;
  playerState_ = NormalizePlayerState(playerState_, *map_);
}

void Game::ApplyPlayerActions(PlayerActions actions, double seconds) {
  playerState_.speed = std::max(playerState_.speed, kBaseSpeed);
  double reachableDistance =
      playerState_.speed * seconds + kAcceleration * seconds * seconds / 2.0;
  playerState_.speed += kAcceleration * seconds;
  const auto goTo = [&](Location nextLoc) {
    const double dx = nextLoc.x - playerState_.location.x;
    const double dy = nextLoc.y - playerState_.location.y;
    const double distance = std::fabs(dx) + std::fabs(dy);
    if (distance <= reachableDistance) {
      playerState_.location = nextLoc;
      reachableDistance -= distance;
    } else {
      playerState_.location.x += dx * reachableDistance / distance;
      playerState_.location.y += dy * reachableDistance / distance;
      reachableDistance = 0.0;
    }
  };
  while (reachableDistance > kEps) {
    const GameMap::Location loc = {
        static_cast<int>(std::round(playerState_.location.x)),
        static_cast<int>(std::round(playerState_.location.y)),
    };
    const double fx = playerState_.location.x - loc.x;
    const double fy = playerState_.location.y - loc.y;
    Location nextLoc = playerState_.location;
    if (actions == kPlayerGoUp) {
      if (std::fabs(fx) < kEps) {
        if (fy <= -kEps) {
          nextLoc = loc;
        } else if (map_->IsHall(loc.Up())) {
          nextLoc = loc.Up();
        }
      } else if (map_->IsHall(loc.Up())) {
        nextLoc = loc;
      }
    } else if (actions == kPlayerGoDown) {
      if (std::fabs(fx) < kEps) {
        if (fy >= kEps) {
          nextLoc = loc;
        } else if (map_->IsHall(loc.Down())) {
          nextLoc = loc.Down();
        }
      } else if (map_->IsHall(loc.Down())) {
        nextLoc = loc;
      }
    } else if (actions == kPlayerGoLeft) {
      if (std::fabs(fy) < kEps) {
        if (fx >= kEps) {
          nextLoc = loc;
        } else if (map_->IsHall(loc.Left())) {
          nextLoc = loc.Left();
        }
      } else if (map_->IsHall(loc.Left())) {
        nextLoc = loc;
      }
    } else if (actions == kPlayerGoRight) {
      if (std::fabs(fy) < kEps) {
        if (fx <= -kEps) {
          nextLoc = loc;
        } else if (map_->IsHall(loc.Right())) {
          nextLoc = loc.Right();
        }
      } else if (map_->IsHall(loc.Right())) {
        nextLoc = loc;
      }
    } else if (actions == (kPlayerGoUp | kPlayerGoRight)) {
      if (fx <= -kEps || fy <= -kEps) {
        nextLoc = loc;
      } else if (fy >= kEps) {
        nextLoc = loc.Up();
      } else if (fx >= kEps) {
        nextLoc = loc.Right();
      } else if (map_->IsHall(loc.Up()) && !map_->IsHall(loc.Right())) {
        nextLoc = loc.Up();
      } else if (!map_->IsHall(loc.Up()) && map_->IsHall(loc.Right())) {
        nextLoc = loc.Right();
      }
    } else if (actions == (kPlayerGoUp | kPlayerGoLeft)) {
      if (fx >= kEps || fy <= -kEps) {
        nextLoc = loc;
      } else if (fy >= kEps) {
        nextLoc = loc.Up();
      } else if (fx <= -kEps) {
        nextLoc = loc.Left();
      } else if (map_->IsHall(loc.Up()) && !map_->IsHall(loc.Left())) {
        nextLoc = loc.Up();
      } else if (!map_->IsHall(loc.Up()) && map_->IsHall(loc.Left())) {
        nextLoc = loc.Left();
      }
    } else if (actions == (kPlayerGoDown | kPlayerGoRight)) {
      if (fx <= -kEps || fy >= kEps) {
        nextLoc = loc;
      } else if (fy <= -kEps) {
        nextLoc = loc.Down();
      } else if (fx >= kEps) {
        nextLoc = loc.Right();
      } else if (map_->IsHall(loc.Down()) && !map_->IsHall(loc.Right())) {
        nextLoc = loc.Down();
      } else if (!map_->IsHall(loc.Down()) && map_->IsHall(loc.Right())) {
        nextLoc = loc.Right();
      }
    } else if (actions == (kPlayerGoDown | kPlayerGoLeft)) {
      if (fx >= kEps || fy >= kEps) {
        nextLoc = loc;
      } else if (fy <= -kEps) {
        nextLoc = loc.Down();
      } else if (fx <= -kEps) {
        nextLoc = loc.Left();
      } else if (map_->IsHall(loc.Down()) && !map_->IsHall(loc.Left())) {
        nextLoc = loc.Down();
      } else if (!map_->IsHall(loc.Down()) && map_->IsHall(loc.Left())) {
        nextLoc = loc.Left();
      }
    }
    if (playerState_.location.x == nextLoc.x &&
        playerState_.location.y == nextLoc.y) {
      playerState_.speed = 0.0;
      break;
    }
    goTo(nextLoc);
  }
  playerState_ = NormalizePlayerState(playerState_, *map_);
}

}  // namespace u7::game
