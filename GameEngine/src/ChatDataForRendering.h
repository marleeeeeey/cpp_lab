#pragma once

#include <string>
#include <vector>

#include "GameSharedObjects/ChatMessage.h"

class ChatDataForRendering {
 public:
  std::string connectionStatus;
  int numberOfConnectedUsers{0};

  void addMessage(const ChatMessage& message);
  const std::vector<ChatMessage>& getChatHistory() const;

 private:
  std::vector<ChatMessage> chatHistory_;
};