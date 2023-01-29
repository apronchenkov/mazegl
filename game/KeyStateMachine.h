//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_GAME_KEYSTATEMACHINE_H_
#define U7_GAME_KEYSTATEMACHINE_H_

#include <bitset>
#include <chrono>
#include <utility>

namespace u7::game {

class KeyStateMachine {
 public:
  enum { UP, DOWN, LEFT, RIGHT, LAST = RIGHT };

  using Keyset = std::bitset<LAST + 1>;

  void Update(std::chrono::steady_clock::time_point now, Keyset keyset) {
    if (keyset.none()) {
      keyset_.reset();
    } else if (internalKeyset_.none()) {
      keyset_ = keyset;
      nextTimestamp_ = now + kDelayBeforeFirstRepeat;
    } else if (now >= nextTimestamp_) {
      keyset_ = keyset;
      nextTimestamp_ = now + kDelayBetweenRepeats;
    }
    internalKeyset_ = keyset;
  }

  Keyset Read() { return std::exchange(keyset_, Keyset{}); }

 private:
  static constexpr std::chrono::steady_clock::duration kDelayBeforeFirstRepeat =
      std::chrono::milliseconds(300);
  static constexpr std::chrono::steady_clock::duration kDelayBetweenRepeats =
      std::chrono::milliseconds(60);

  std::chrono::steady_clock::time_point nextTimestamp_;
  Keyset keyset_;
  Keyset internalKeyset_;
};

}  // namespace u7::game

#endif  // U7_KEYSTATEMACHINE_H_
