#pragma once

#include <glm/ext/scalar_constants.hpp>
#include <glm/vec2.hpp>

class GeomUtils {
 public:
  static glm::vec2 safeNormalize(glm::vec2 v, float eps = glm::epsilon<float>());

  static float approach(float current, float target, float maxDelta);

  static glm::vec2 approachVec2(const glm::vec2& current, const glm::vec2& target, float maxDelta);

  static float wrapCoord(float v, float maxExclusive);
};
