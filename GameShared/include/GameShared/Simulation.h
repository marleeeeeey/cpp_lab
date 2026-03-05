#pragma once

#include <glm/glm.hpp>

#include "GameShared/Player.h"

void simulatePlayer(PlayerState& state, float dtSeconds, const glm::vec2& input,
                    float worldWidth, float worldHeight);