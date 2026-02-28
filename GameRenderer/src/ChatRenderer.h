#pragma once

#include <vector>

#include "GameRenderer/IChatRenderer.h"
#include "GameShared/ChatMessage.h"

class ChatRenderer : public IChatRenderer {
 public:
  void addMessage(const ChatMessage& message) override;
  void render() override;

 private:
  std::vector<ChatMessage> chatHistory_;
};