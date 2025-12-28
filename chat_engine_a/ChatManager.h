#pragma once

#include "DataForRendering.h"

// Business logic for the chat engine.
class ChatManager {
  DataForRendering dataForRendering;

 public:
  void init();
  void iterate(double elapsed);
  const DataForRendering& getOutputDataForRendering() const;
};
