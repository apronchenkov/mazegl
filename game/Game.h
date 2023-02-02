//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_GAME_GAME_H_
#define U7_GAME_GAME_H_

#include "game/GameMap.h"

#include <bitset>
#include <memory>

namespace u7::game {

class Game {
 public:
  struct Location {
    double x = 0.0;
    double y = 0.0;

    bool IsCloseTo(const Location& rhs) const;
  };

  struct PlayerState {
    Location location = {};
    bool touchedExit = false;
  };

  enum PlayerAction {
    GO_UP,
    GO_LEFT,
    GO_RIGHT,
    GO_DOWN,
    LAST = GO_DOWN,
  };

  using PlayerActions = std::bitset<PlayerAction::LAST + 1>;

  explicit Game(std::shared_ptr<const GameMap> map);

  const GameMap& GetGameMap() const { return *map_; }

  PlayerState GetPlayerState() const { return playerState_; }

  void ApplyPlayerActions(PlayerActions actions, double seconds);

 private:
  std::shared_ptr<const GameMap> map_;
  PlayerState playerState_;
};

}  // namespace u7::game

#endif  // U7_GAME_GAME_H_
