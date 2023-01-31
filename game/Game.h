//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_GAME_GAME_H_
#define U7_GAME_GAME_H_

#include "game/GameMap.h"

#include <memory>

namespace u7::game {

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
  explicit Game(std::shared_ptr<const GameMap> map);

  const GameMap& GetGameMap() const { return *map_; }

  GameState GetGameState() const { return gameState_; }

  GameMap::Location GetPlayerLocation() const { return playerLocation_; }

  void PerformPlayerAction(PlayerAction action);

 private:
  std::shared_ptr<const GameMap> map_;
  GameMap::Location playerLocation_;
  GameState gameState_ = GameState::IN_PROGRESS;
};

}  // namespace u7::game

#endif  // U7_GAME_GAME_H_
