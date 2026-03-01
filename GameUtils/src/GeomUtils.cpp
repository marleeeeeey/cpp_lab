#include "GameUtils/GeomUtils.h"

#include <glm/glm.hpp>

glm::vec2 GeomUtils::safeNormalize(glm::vec2 v, float eps) {
  if (glm::length(v) <= eps) return glm::vec2(0.0f);
  return glm::normalize(v);
}