//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#include "palettes/Palettes.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace u7::palettes {

Colour3f GetColour(float value, std::span<const Colour3f> palette) {
  if (palette.empty()) {
    return Colour3f{0, 0, 0};
  }
  if (palette.size() == 1) {
    return palette[0];
  }
  size_t idx1;
  size_t idx2;
  float fraction = 0;
  if (value <= 0) {
    idx1 = idx2 = 0;
  } else if (value >= 1) {
    idx1 = idx2 = palette.size() - 1;
  } else {
    value *= static_cast<float>(palette.size() - 1);
    idx1 = static_cast<size_t>(value);
    idx2 = idx1 + 1;
    fraction = value - static_cast<float>(idx1);
  }
  return Colour3f{
      (palette[idx2].r - palette[idx1].r) * fraction + palette[idx1].r,
      (palette[idx2].g - palette[idx1].g) * fraction + palette[idx1].g,
      (palette[idx2].b - palette[idx1].b) * fraction + palette[idx1].b};
}

Colour3f GetCubehelixColour(float value, float startColour,
                            float numberOfColourRotations, float hue,
                            float gamma) {
  constexpr float kPi = 3.1415926535897;
  const float phi =
      2 * kPi * (startColour / 3 + numberOfColourRotations * value);
  const float cosPhi = std::cos(phi);
  const float sinPhi = std::sin(phi);
  const float lambdaGamma = std::pow(value, gamma);
  const float alpha = hue * lambdaGamma * (1 - lambdaGamma) / 2;
  return Colour3f{
      std::clamp(lambdaGamma + alpha * (-0.14861f * cosPhi + 1.78277f * sinPhi),
                 0.0f, 1.0f),
      std::clamp(lambdaGamma + alpha * (-0.29227f * cosPhi - 0.90649f * sinPhi),
                 0.0f, 1.0f),
      std::clamp(lambdaGamma + alpha * (1.97294f * cosPhi), 0.0f, 1.0f)};
}

std::vector<Colour3f> GetCubehelixPalette(size_t n, float begin, float end,
                                          float startColour,
                                          float numberOfColourRotations,
                                          float hue, float gamma) {
  if (n == 0) {
    return {};
  }
  if (n == 1) {
    return {GetCubehelixColour(begin, startColour, numberOfColourRotations, hue,
                               gamma)};
  }
  std::vector<Colour3f> result(n);
  for (size_t i = 0; i < n; ++i) {
    result[i] = GetCubehelixColour(
        begin + static_cast<float>((end - begin) * i) / (n - 1), startColour,
        numberOfColourRotations, hue, gamma);
  }
  return result;
}

std::span<const Colour3f> GetHeatmap5Palette() {
  static constexpr std::array result = {
      Colour3f{0, 0, 1},  // blue
      Colour3f{0, 1, 1},  // cyan
      Colour3f{0, 1, 0},  // green
      Colour3f{1, 1, 0},  // yellow
      Colour3f{1, 0, 0},  // red
  };
  return result;
}

}  // namespace u7::palettes
