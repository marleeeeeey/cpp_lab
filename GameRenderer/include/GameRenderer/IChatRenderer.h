#pragma once

#include <functional>

#include "GameSharedObjects/ChatMessage.h"
#include "IRenderer.h"

// Class to render a chat window
class IChatRenderer : public IRenderer {
 public:
  // -------------
  // Signatures
  // -------------

  using OnMessageSentCallback = std::function<void(const std::string&)>;

  // ----------
  // Factory
  // ----------

  static std::unique_ptr<IChatRenderer> create();
  virtual ~IChatRenderer() override = default;

  // -----------
  // Interface
  // -----------

  virtual void addMessage(const ChatMessage& message) = 0;
  OnMessageSentCallback onMessageSentCallback;
};