#pragma once

#include "DataForRendering.h"
#include "chat_network/ChatServer.h"

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
