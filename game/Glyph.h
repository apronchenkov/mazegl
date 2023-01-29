//
// Created by Alexander G. Pronchenkov on 28.01.2023.
//
#ifndef U7_GAME_GLYPH_H_
#define U7_GAME_GLYPH_H_

#include <cstdint>
#include <span>
#include <string_view>

namespace u7::game {

struct Glyph {
  struct Pxl {
    int8_t x, y;
  };
  Pxl base;
  Pxl dimension;
  std::span<const Pxl> pxls;
};

Glyph GetStandardGlyph(std::string_view glyphName);

}  // namespace u7::game

#endif  // U7_GAME_GLYPH_H_
