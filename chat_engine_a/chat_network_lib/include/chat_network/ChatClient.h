#pragma once

#include <memory>
#include <string>

#include "chat_network/Handlers.h"

class ChatClient {
  struct Impl;
  std::unique_ptr<Impl> pimpl;

 public:
  ChatClient();
  ~ChatClient();

 public:
  void start(const std::string& host, short port,
             MessageHandler onMsg, ErrorHandler onErr);
  void stop();
  void poll();
  void send(const std::string& msg);
  bool isConnected() const;
};
