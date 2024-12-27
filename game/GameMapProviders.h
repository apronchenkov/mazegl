//
// Created by Alexander G. Pronchenkov on 27.12.2024.
//
#ifndef U7_GAME_GAMEMAP_PROVIDERS_H_
#define U7_GAME_GAMEMAP_PROVIDERS_H_

#include "game/GameMap.h"
#include "maze/Maze.h"

#include <cstddef>
#include <memory>
#include <random>

namespace u7::game {

class GameMapProvider {
 public:
  virtual GameMapPtr Gen(int suggestedWidth, int suggestedHeight) = 0;

  virtual ~GameMapProvider() = default;
  GameMapProvider() = default;
  GameMapProvider(const GameMapProvider&) = delete;
  GameMapProvider operator=(const GameMapProvider&) = delete;
};

using GameMapProviderPtr = std::shared_ptr<GameMapProvider>;

GameMapProviderPtr MakeMazeMapProvider(maze::GenMazeOptions options,
                                       std::seed_seq* seedSeq);

}  // namespace u7::game

#endif  // U7_GAME_GAMEMAP_PROVIDERS_H_
