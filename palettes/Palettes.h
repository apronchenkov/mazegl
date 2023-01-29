//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_PALETTES_PALETTES_H_
#define U7_PALETTES_PALETTES_H_
#include <span>
#include <vector>

namespace u7::palettes {

struct Colour3f {
  float r, g, b;
};

// Returns an interpolated colour corresponding to the given value and palette.
Colour3f GetColour(float value, std::span<const Colour3f> palette);

// Based on: https://arxiv.org/pdf/1108.5083.pdf
//
// Args:
//   value: A value (0 <= value <= 1).
//   startColour: The start colour. This is the direction of the predominant
//       colour deviation from black at the start of the colour scheme
//       (0 <= start <= 3).
//   numberOfColourRotations: The number of R, G, B rotations in colour that are
//       made from the start (i.e. black) to the end (i.e. white) of the colour
//       scheme.
//   hue: A hue parameter, which controls how saturated the colours are.
//   gamma: A ‘gamma factor’ to emphasise either low intensity values
//       (0 <= gamme < 1), or high intensity values (gamme >= 1).
//
Colour3f GetCubehelixColour(float value, float startColour = 0.5f,
                            float numberOfColourRotations = -1.5f,
                            float hue = 1.2f, float gamma = 1.0f);

// Return a list of colours defining a cubehelix palette.
//
// Args:
//   n: The palette size.
//   begin: Value of the first colour in the palette.
//   end: Value of the last colour in the palette.
//   startColour: See GetCubehelixColour.
//   numberOfColourRotations: See GetCubehelixColour.
//   hue: See GetCubehelixColour.
//   gamma: See GetCubehelixColour.
std::vector<Colour3f> GetCubehelixPalette(size_t n, float begin = 0.0f,
                                          float end = 1.0f,
                                          float startColour = 0.5f,
                                          float numberOfColourRotations = -1.5f,
                                          float hue = 1.2f, float gamma = 1.0f);

// Returns a list of colours defining a heatmap5 palette.
const std::span<const Colour3f> GetHeatmap5Palette();

}  // namespace u7::palettes

#endif  // U7_PALETTES_PALETTES_H_
