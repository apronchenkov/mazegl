//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "game/Game.h"

namespace u7::game {

Game::Game(std::shared_ptr<const GameMap> map)
    : map_(std::move(map)),
      playerLocation_(map_->GetEntranceLocation()),
      gameState_(map_->GetEntranceLocation() == map_->GetExitLocation()
                     ? GameState::COMPLETE
                     : GameState::IN_PROGRESS) {}

void Game::PerformPlayerAction(PlayerAction action) {
  GameMap::Location newPlayerLocation;
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
