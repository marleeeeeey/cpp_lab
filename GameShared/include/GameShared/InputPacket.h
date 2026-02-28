#pragma once

#include <cstdint>

struct InputPacket {
  float moveX;  // -1..1
  float moveY;  // -1..1
  uint32_t inputTick;
};