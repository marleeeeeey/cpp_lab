#include "ChatDataForRendering.h"

void ChatDataForRendering::addMessage(const ChatMessage& message) {
  chatHistory_.push_back(message);
}

const std::vector<ChatMessage>& ChatDataForRendering::getChatHistory() const {
  return chatHistory_;
}