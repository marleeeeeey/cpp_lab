#pragma once

#include <chrono>
#include <cstdint>
#include <span>

using PayloadType = std::uint16_t;
using PayloadView = std::span<const std::uint8_t>;
using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
