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
  using PlayerActions = std::bitset<4>;

  static constexpr PlayerActions kPlayerGoUp{0b0001};
  static constexpr PlayerActions kPlayerGoDown{0b0010};
  static constexpr PlayerActions kPlayerGoLeft{0b0100};
  static constexpr PlayerActions kPlayerGoRight{0b1000};

  struct Location {
    double x = 0.0;
    double y = 0.0;

    Location() = default;
    Location(const GameMap::Location& rhs) : x(rhs.x), y(rhs.y) {}
  };

  struct PlayerState {
    Location location = {};
    bool touchedExit = false;
    double speed = 0.0;
  };

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
