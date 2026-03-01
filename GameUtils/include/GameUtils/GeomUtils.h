#pragma once

#include <glm/ext/scalar_constants.hpp>
#include <glm/vec2.hpp>

class GeomUtils {
 public:
  static glm::vec2 safeNormalize(glm::vec2 v, float eps = glm::epsilon<float>());
};
