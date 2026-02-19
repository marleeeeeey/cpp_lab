#pragma once

#include <functional>

#include "GameSharedObjects/ChatMessage.h"
#include "IRenderer.h"

class IChatRenderer : public IRenderer {
 public:
  virtual ~IChatRenderer() = default;

  virtual void addMessage(const ChatMessage& message) = 0;

  std::string connectionStatus;
  int numberOfConnectedUsers{0};
  using OnMessageSentCallback = std::function<void(const std::string&)>;
  OnMessageSentCallback onMessageSentCallback;
};