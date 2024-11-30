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
  using PlayerActions = std::bitset<6>;

  static constexpr PlayerActions kPlayerGoUp{0b000001};
  static constexpr PlayerActions kPlayerGoDown{0b000010};
  static constexpr PlayerActions kPlayerGoLeft{0b000100};
  static constexpr PlayerActions kPlayerGoRight{0b001000};
  static constexpr PlayerActions kPlayerGoMask{0b001111};

  static constexpr PlayerActions kPlayerAsk1{0b010000};
  static constexpr PlayerActions kPlayerAsk2{0b100000};
  static constexpr PlayerActions kPlayerAskMask{0b110000};

  struct Location {
    double x = 0.0;
    double y = 0.0;

    Location() = default;
    /* implicit */ Location(const GameMap::Location& rhs)
        : x(rhs.x), y(rhs.y) {}
  };

  struct PlayerState {
    Location location = {};
    bool touchedExit = false;
    double speed = 0.0;
    bool ask1 = false;
    bool ask2 = false;
  };

  explicit Game(std::shared_ptr<const GameMap> map);

  [[nodiscard]] const GameMap& GetGameMap() const { return *map_; }

  [[nodiscard]] PlayerState GetPlayerState() const { return playerState_; }

  void ApplyPlayerActions(PlayerActions actions, double seconds);

 private:
  std::shared_ptr<const GameMap> map_;
  PlayerState playerState_;
};

}  // namespace u7::game

#endif  // U7_GAME_GAME_H_
