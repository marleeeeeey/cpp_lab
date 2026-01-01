#pragma once

#include <memory>
#include <string>

#include "network_connection/Handlers.h"

// This class responsible for connecting to the server.
// It wraps the ChatConnection class and reuses it for reading and writing messages.
class ChatClient {
  struct Impl;
  std::unique_ptr<Impl> pimpl;

 public:
  ChatClient();
  ~ChatClient();

  // Start method may be called multiple times to reconnect.
  void start(const std::string& host, short port, MessageHandler onMsg, ErrorHandler onErr);

  // Destroys the connection and stops the underlying socket. May be reused by the `start` method again.
  void stop();

  // Should be called every frame. It initiates asynchronous read operations for the connection.
  void poll();

  // Send a message to the server.
  void send(const std::string& msg);

  // Check if the connection is active.
  bool isConnected() const;
};
