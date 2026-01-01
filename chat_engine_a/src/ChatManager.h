#pragma once

#include "DataForRendering.h"
#include "chat_network/ChatClient.h"
#include "chat_network/ChatServer.h"

// Business logic for the chat engine.
class ChatManager {
  DataForRendering dataForRendering;
  ChatServer chatServer;
  ChatClient chatClient;

 public:
  void init();
  void iterate(double elapsed);
  void stop();
  void sendMessage(const std::string& msg);  // Actual for sending messages to the server.
  const DataForRendering& getOutputDataForRendering() const;
};
