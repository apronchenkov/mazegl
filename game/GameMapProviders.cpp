#include "game/GameMapProviders.h"

#include "lua_bridge/Utils.h"
#include "maze/Maze.h"

#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace u7::game {
namespace {

using ::u7::lua_bridge::LuaStatePtr;
using ::u7::lua_bridge::MakeLuaState;
using ::u7::maze::GenMazeOptions;
using ::u7::maze::Maze;

class MazeMapProvider final : public GameMapProvider {
 public:
  MazeMapProvider(GenMazeOptions options, std::seed_seq* seedSeq)
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

namespace {

class LuaMapProvider final : public GameMapProvider {
 public:
  explicit LuaMapProvider(LuaStatePtr L) : L_(std::move(L)) {}

  GameMapPtr Gen(int width, int height) final {
    if (L_ == nullptr) {
      throw std::runtime_error("LuaBasedMapProvider broken");
    }
    lua_getglobal(L_.get(), "next_game_map");
    lua_pushnumber(L_.get(), width);
    lua_pushnumber(L_.get(), height);
    if (lua_pcall(L_.get(), 2, 1, 0) != 0) {
      std::ostringstream message;
      message << "error running function `next_game_map`: "
              << lua_tostring(L_.get(), -1);
      L_.reset();
      throw std::runtime_error(std::move(message).str());
    }
    size_t len;
    const char* data = lua_tolstring(L_.get(), -1, &len);
    if (data == nullptr) {
      L_.reset();
      throw std::runtime_error(
          "error running function `next_game_map`: function must return a "
          "string");
    }

    try {
      auto result = ParseMapData(std::string_view(data, len));
      lua_pop(L_.get(), -1);
      return result;
    } catch (...) {
      L_.reset();
      throw;
    }
  }

  static GameMapPtr ParseMapData(std::string_view data) {
    if (data.empty()) {
      data = "+-";
    }
    std::vector<std::string_view> rows;
    while (!data.empty()) {
      size_t i = 0;
      while (i < data.size() && data[i] != '\n') {
        ++i;
      }
      rows.push_back(data.substr(0, i));
      data.remove_prefix(i + (i < data.size()));
    }
    std::reverse(rows.begin(), rows.end());
    size_t n = rows.size();
    size_t m = 0;
    for (const auto& row : rows) {
      m = std::max(m, row.size());
    }
    if (n > std::numeric_limits<int>::max() ||
        m > std::numeric_limits<int>::max()) {
      throw std::runtime_error("the map is too large");
    }
    Maze maze(n, m);
    GameMap::Location entrance{0, 0};
    GameMap::Location exit{0, 0};
    for (size_t y = 0; y < rows.size(); ++y) {
      for (size_t x = 0; x < rows[y].size(); ++x) {
        if (rows[y][x] == '+') {
          entrance.y = y;
          entrance.x = x;
        } else if (rows[y][x] == '-') {
          exit.y = y;
          exit.x = x;
        } else if (rows[y][x] == '*') {
          exit.y = y;
          exit.x = x;
        }
        maze.UnsafeAt(y, x) = (rows[y][x] != '#');
      }
    }
    return std::make_shared<GameMap>(std::move(maze), entrance, exit);
  }

 private:
  LuaStatePtr L_;
};

}  // namespace

GameMapProviderPtr MakeLuaMapProvider(std::string_view luaFile) {
  auto L = MakeLuaState();
  luaL_openlibs(L.get());
  if (0 != luaL_dofile(L.get(), std::string(luaFile).c_str())) {
    std::ostringstream message;
    message << "luaL_dofile(" << std::quoted(luaFile)
            << "): " << lua_tostring(L.get(), -1);
    throw std::runtime_error(std::move(message).str());
  }
  return std::make_shared<LuaMapProvider>(std::move(L));
}

}  // namespace u7::game
