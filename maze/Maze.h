//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_MAZE_MAZE_H_
#define U7_MAZE_MAZE_H_

#pragma once
#include "algorithm/Matrix.h"

#include <functional>

namespace u7::maze {

using Maze = ::u7::algorithm::Matrix<bool>;

using Rng = std::function<int()>;

struct GenMazeOptions {
  bool noLoops = true;
  bool noSmallSquares = true;
  // Limit the number of halls per hamming spheres.
  int limitDensityR = 0;             // 3
  size_t limitDensityThreshold = 0;  // 5, max: 2 * r * (r + 1) + 1

  // Remove dead ends of length=1.
  bool pruneStubs = true;
};

Maze GenMaze(int n, int m, Rng rng, GenMazeOptions options = {});

}  // namespace u7::maze

#endif  // U7_MAZE_MAZE_H_
