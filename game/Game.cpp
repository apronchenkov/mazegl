//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/Game.h"

#include <cmath>

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
  constexpr double kSpeed = 5.0;
  while (seconds > 0) {
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
    }

    if (actions == (kPlayerGoUp | kPlayerGoRight)) {
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
    }

    if (actions == (kPlayerGoUp | kPlayerGoLeft)) {
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
    }

    if (actions == (kPlayerGoDown | kPlayerGoRight)) {
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
    }

    if (actions == (kPlayerGoDown | kPlayerGoLeft)) {
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

    const double xDelta = nextLoc.x - playerState_.location.x;
    const double yDelta = nextLoc.y - playerState_.location.y;
    const double delta = std::fabs(xDelta) + std::fabs(yDelta);
    if (delta < kEps) {
      break;
    }
    const double scale = std::min(seconds * kSpeed, delta) / delta;
    seconds -= scale * delta / kSpeed;
    playerState_.location.x += scale * xDelta;
    playerState_.location.y += scale * yDelta;
    playerState_ = NormalizePlayerState(playerState_, *map_);
  }
}

}  // namespace u7::game
