#include "GameUtils/GeomUtils.h"

#include <cmath>
#include <glm/glm.hpp>

glm::vec2 GeomUtils::safeNormalize(glm::vec2 v, float eps) {
  if (glm::length(v) <= eps) return glm::vec2(0.0f);
  return glm::normalize(v);
}

float GeomUtils::approach(float current, float target, float maxDelta) {
  const float delta = target - current;
  if (delta > maxDelta) return current + maxDelta;
  if (delta < -maxDelta) return current - maxDelta;
  return target;
}

glm::vec2 GeomUtils::approachVec2(const glm::vec2& current, const glm::vec2& target, float maxDelta) {
  return glm::vec2(
      approach(current.x, target.x, maxDelta),
      approach(current.y, target.y, maxDelta));
}

float GeomUtils::wrapCoord(float v, float maxExclusive) {
  if (maxExclusive <= 0.0f) return 0.0f;
  v = std::fmod(v, maxExclusive);
  if (v < 0.0f) v += maxExclusive;
  return v;
}