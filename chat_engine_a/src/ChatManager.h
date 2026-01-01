#pragma once

#include "DataForRendering.h"
#include "chat_network/ChatClient.h"
#include "chat_network/ChatServer.h"

// Business logic for the chat engine.
class ChatManager {
 public:
  enum Mode : uint8_t {
    None = 0,
    Client = 1 << 0,
    Server = 1 << 1,
    Both = Client | Server,
  };

 private:
  DataForRendering dataForRendering;
  ChatServer chatServer;
  ChatClient chatClient;
  uint8_t modeMask_ = Mode::None;

 public:
  void init(uint8_t modeMask);  // See enum Mode for details
  void iterate(double elapsed);
  void stop();
  void sendMessage(const std::string& msg);  // Actual for sending messages to the server.
  const DataForRendering& getOutputDataForRendering() const;

 private:
  void startServer();
  void connectToServer();
};
