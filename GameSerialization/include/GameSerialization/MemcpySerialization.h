#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "GlobalTypes/GlobalTypes.h"

namespace GameSerialization {

template <class T>
concept MemcpySerializable =
    std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;

template <MemcpySerializable T>
inline std::vector<std::uint8_t> serializeMemcpy(const T& item) {
  std::vector<std::uint8_t> buffer(sizeof(T));
  std::memcpy(buffer.data(), &item, sizeof(T));
  return buffer;
}

template <MemcpySerializable T>
inline T deserializeMemcpy(PayloadView payload) {
  if (payload.size() != sizeof(T)) {
    throw std::runtime_error("deserializeMemcpy: payload size mismatch");
  }

  T item{};
  std::memcpy(&item, payload.data(), sizeof(T));
  return item;
}

}  // namespace GameSerialization