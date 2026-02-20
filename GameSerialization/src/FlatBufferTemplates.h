#pragma once

#include "FlatBufferWrappers.h"

namespace FlatBufferTemplates {

template <typename T>
std::vector<uint8_t> serialize(const T& object) {
  flatbuffers::FlatBufferBuilder builder;
  auto fbObject = FlatBufferWrappers::serialize(builder, object);
  builder.Finish(fbObject);
  return std::vector<uint8_t>(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());
}

}  // namespace FlatBufferTemplates