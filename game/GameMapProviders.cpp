#include "game/GameMapProviders.h"

#include "maze/Maze.h"

#include <random>

namespace u7::game {
namespace {

class MazeMapProvider final : public GameMapProvider {
 public:
  MazeMapProvider(maze::GenMazeOptions options, std::seed_seq* seedSeq)
      : options_(options), rng_(*seedSeq) {}

  GameMapPtr Gen(int width, int height) final {
    auto maze = GenMaze(height, width, [&] { return rng_(); }, options_);
    GameMap::Location entrance;
    GameMap::Location exit;
    int entranceD = width + height;
    int exitD = 0;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        if (maze.UnsafeAt(y, x)) {
          const int d = x + y;
          if (entranceD > d) {
            entrance.x = x;
            entrance.y = y;
            entranceD = d;
          }
          if (exitD < d) {
            exit.x = x;
            exit.y = y;
            exitD = d;
          }
        }
      }
    }
    return std::make_shared<GameMap>(std::move(maze), entrance, exit);
  }

 private:
  maze::GenMazeOptions options_;
  std::mt19937 rng_;
};

}  // namespace

GameMapProviderPtr MakeMazeMapProvider(maze::GenMazeOptions options,
                                       std::seed_seq* seedSeq) {
  return std::make_shared<MazeMapProvider>(std::move(options), seedSeq);
}

}  // namespace u7::game
