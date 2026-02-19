#pragma once

#include <functional>
#include <string>
#include <vector>

#include "GameSharedObjects/ChatMessage.h"
#include "IChatRenderer.h"

class ChatRenderer : public IChatRenderer {
 public:
  void addMessage(const ChatMessage& message) override;
  void render() override;

 private:
  std::vector<ChatMessage> chatHistory_;
};