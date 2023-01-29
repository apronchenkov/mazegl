//
// Created by Alexander G. Pronchenkov on 28.01.2023.
//
#include "game/Glyph.h"

#include <array>

namespace u7::game {

Glyph GetStandardGlyph(std::string_view glyphName) {
  using Pxl = Glyph::Pxl;
  static constexpr std::array digit0Pxls = {
      Pxl{0, 1}, Pxl{0, 2}, Pxl{0, 3}, Pxl{0, 4}, Pxl{0, 5},
      Pxl{1, 6}, Pxl{2, 6}, Pxl{3, 5}, Pxl{3, 4}, Pxl{3, 3},
      Pxl{3, 2}, Pxl{3, 1}, Pxl{2, 0}, Pxl{1, 0},
  };
  static constexpr std::array digit1Pxls = {
      Pxl{0, 4}, Pxl{1, 5}, Pxl{2, 6}, Pxl{2, 5}, Pxl{2, 4},
      Pxl{2, 3}, Pxl{2, 2}, Pxl{2, 1}, Pxl{2, 0},
  };
  static constexpr std::array digit2Pxls = {
      Pxl{0, 5}, Pxl{1, 6}, Pxl{2, 6}, Pxl{3, 5}, Pxl{3, 4},
      Pxl{2, 3}, Pxl{1, 2}, Pxl{0, 1}, Pxl{0, 0}, Pxl{1, 0},
      Pxl{1, 0}, Pxl{2, 0}, Pxl{3, 0}};
  static constexpr std::array digit3Pxls = {
      Pxl{0, 5}, Pxl{1, 6}, Pxl{2, 6}, Pxl{3, 5}, Pxl{3, 4}, Pxl{2, 3},
      Pxl{3, 2}, Pxl{3, 1}, Pxl{2, 0}, Pxl{1, 0}, Pxl{0, 1},
  };
  static constexpr std::array digit4Pxls = {
      Pxl{0, 6}, Pxl{0, 5}, Pxl{0, 4}, Pxl{0, 3}, Pxl{1, 3},
      Pxl{2, 3}, Pxl{2, 3}, Pxl{3, 3}, Pxl{3, 6}, Pxl{3, 5},
      Pxl{3, 4}, Pxl{3, 2}, Pxl{3, 1}, Pxl{3, 0},
  };
  static constexpr std::array digit5Pxls = {
      Pxl{3, 6}, Pxl{2, 6}, Pxl{1, 6}, Pxl{0, 6}, Pxl{0, 5},
      Pxl{0, 4}, Pxl{1, 4}, Pxl{2, 4}, Pxl{3, 3}, Pxl{3, 2},
      Pxl{3, 1}, Pxl{2, 0}, Pxl{1, 0}, Pxl{0, 1},
  };
  static constexpr std::array digit6Pxls = {
      Pxl{3, 5}, Pxl{2, 6}, Pxl{1, 6}, Pxl{0, 5}, Pxl{0, 4},
      Pxl{0, 3}, Pxl{0, 2}, Pxl{0, 1}, Pxl{1, 0}, Pxl{2, 0},
      Pxl{3, 1}, Pxl{3, 2}, Pxl{2, 3}, Pxl{1, 3},
  };
  static constexpr std::array digit7Pxls = {
      Pxl{0, 6}, Pxl{1, 6}, Pxl{2, 6}, Pxl{3, 6}, Pxl{3, 5},
      Pxl{3, 4}, Pxl{2, 3}, Pxl{2, 2}, Pxl{1, 1}, Pxl{1, 0},
  };
  static constexpr std::array digit8Pxls = {
      Pxl{0, 4}, Pxl{0, 5}, Pxl{1, 6}, Pxl{2, 6}, Pxl{3, 5},
      Pxl{3, 4}, Pxl{2, 3}, Pxl{1, 3}, Pxl{0, 2}, Pxl{0, 1},
      Pxl{1, 0}, Pxl{2, 0}, Pxl{3, 1}, Pxl{3, 2},
  };
  static constexpr std::array digit9Pxls = {
      Pxl{2, 3}, Pxl{1, 3}, Pxl{0, 4}, Pxl{0, 5}, Pxl{1, 6},
      Pxl{2, 6}, Pxl{3, 5}, Pxl{3, 4}, Pxl{3, 3}, Pxl{3, 2},
      Pxl{3, 1}, Pxl{2, 0}, Pxl{1, 0}, Pxl{0, 1},
  };
  if (glyphName == " ") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = {}};
  }
  if (glyphName == "0") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit0Pxls};
  }
  if (glyphName == "1") {
    return Glyph{.base = {0, 0}, .dimension = {4, 8}, .pxls = digit1Pxls};
  }
  if (glyphName == "2") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit2Pxls};
  }
  if (glyphName == "3") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit3Pxls};
  }
  if (glyphName == "4") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit4Pxls};
  }
  if (glyphName == "5") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit5Pxls};
  }
  if (glyphName == "6") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit6Pxls};
  }
  if (glyphName == "7") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit7Pxls};
  }
  if (glyphName == "8") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit8Pxls};
  }
  if (glyphName == "9") {
    return Glyph{.base = {0, 0}, .dimension = {5, 8}, .pxls = digit9Pxls};
  }
  return Glyph{};
}

}  // namespace u7::game
