#pragma once

#include "ChatServer.h"
#include "DataForRendering.h"

// Business logic for the chat engine.
class ChatManager {
  DataForRendering dataForRendering;
  ChatServer chatServer;

 public:
  void init();
  void iterate(double elapsed);
  void stop();
  const DataForRendering& getOutputDataForRendering() const;
};
