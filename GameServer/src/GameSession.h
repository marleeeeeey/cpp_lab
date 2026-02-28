#pragma once

#include <cstdint>
#include <mutex>

struct GameSession {
  std::mutex mutex;
  uint32_t tick = 0;
  std::vector<PerSocketData*> perSocketDatas;
};