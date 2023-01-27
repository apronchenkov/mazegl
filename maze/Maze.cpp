//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "maze/Maze.h"

#include <queue>
#include <tuple>
#include <vector>

namespace u7::maze {
namespace {

struct Cell {
  int weight;
  int i;
  int j;
  auto tie() const { return std::tie(weight, i, j); }
};

struct CellOrder {
  bool operator()(const Cell& lhs, const Cell& rhs) const {
    return lhs.tie() < rhs.tie();
  }
};

}  // namespace

Maze GenMaze(int n, int m, Rng rng, GenMazeOptions options) {
  std::priority_queue<Cell, std::vector<Cell>, CellOrder> queue;
  Maze result(n, m);
  Maze marked(n, m);
  const auto enqueue = [&](int i, int j) {
    if (i >= 0 && i < n && j >= 0 && j < m && !marked.UnsafeAt(i, j)) {
      marked.UnsafeAt(i, j) = true;
      queue.push(Cell{rng(), i, j});
    }
  };
  const auto at = [&](int i, int j) {
    return (i >= 0 && i < n && j >= 0 && j < m && result.UnsafeAt(i, j));
  };
  const auto deg = [&](int i, int j) {
    return at(i - 1, j) + at(i, j - 1) + at(i, j + 1) + at(i + 1, j);
  };
  result.Fill(false);
  marked.Fill(false);
  const int i0 = n / 2;
  const int j0 = m / 2;
  enqueue(i0, j0);
  while (!queue.empty()) {
    const auto [_, i, j] = queue.top();
    queue.pop();
    if (options.noLoops) {
      if (deg(i, j) > 1) {
        continue;
      }
    } else if (options.noSmallSquares) {
      if ((at(i - 1, j) && at(i - 1, j - 1) && at(i, j - 1)) ||
          (at(i, j - 1) && at(i + 1, j - 1) && at(i + 1, j)) ||
          (at(i + 1, j) && at(i + 1, j + 1) && at(i, j + 1)) ||
          (at(i, j + 1) && at(i - 1, j + 1) && at(i - 1, j))) {
        continue;
      }
    }
    size_t counter = 0;
    for (int r = 1; r <= options.limitDensityR; ++r) {
      for (int s = 0; s < r; ++s) {
        counter += at(i - r + s, j - s) + at(i + s, j - r + s) +
                   at(i + r - s, j + s) + at(i - s, j + r - s);
      }
    }
    if (counter > options.limitDensityThreshold) {
      continue;
    }
    result.UnsafeAt(i, j) = true;
    enqueue(i - 1, j);
    enqueue(i, j - 1);
    enqueue(i, j + 1);
    enqueue(i + 1, j);
  }
  if (options.pruneStubs) {
    marked.Fill(false);
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
        if (result.UnsafeAt(i, j)) {
          marked.UnsafeAt(i, j) = (deg(i, j) == 1);
        }
      }
    }
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
        if (!result.UnsafeAt(i, j)) {
          continue;
        }
        int hallCnt = 0;
        int markCnt = 0;
        if (i > 0) {
          hallCnt += result.UnsafeAt(i - 1, j);
          markCnt += marked.UnsafeAt(i - 1, j);
        }
        if (j > 0) {
          hallCnt += result.UnsafeAt(i, j - 1);
          markCnt += marked.UnsafeAt(i, j - 1);
        }
        if (j + 1 < m) {
          hallCnt += result.UnsafeAt(i, j + 1);
          markCnt += marked.UnsafeAt(i, j + 1);
        }
        if (i + 1 < n) {
          hallCnt += result.UnsafeAt(i + 1, j);
          markCnt += marked.UnsafeAt(i + 1, j);
        }
        if (markCnt == 0 || hallCnt - markCnt <= 1) {
          continue;
        }
        if (i > 0 && marked.UnsafeAt(i - 1, j)) {
          result.UnsafeAt(i - 1, j) = false;
        }
        if (j > 0 && marked.UnsafeAt(i, j - 1)) {
          result.UnsafeAt(i, j - 1) = false;
        }
        if (j + 1 < m && marked.UnsafeAt(i, j + 1)) {
          result.UnsafeAt(i, j + 1) = false;
        }
        if (i + 1 < n && marked.UnsafeAt(i + 1, j)) {
          result.UnsafeAt(i + 1, j) = false;
        }
      }
    }
  }
  return result;
}

}  // namespace u7::maze
