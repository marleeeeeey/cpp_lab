#pragma once

#include <memory>

#include "network_connection/Handlers.h"

// Class is responsible for accepting incoming connections and delivering messages to the appropriate clients.
// Can be run in the main thread. All methods are non-blocking. Handlers are called in the same thread.
class ChatServer {
  struct Impl;
  std::unique_ptr<Impl> pimpl;

 public:
  // Ctor/Dtor must be declared here and defined in the cpp file when data under the unique_ptr is defined.
  // Because unique_ptr should know the size of the object.
  ChatServer();
  ~ChatServer();

 public:
  // Start method may be called multiple times to reconnect.
  void start(short port, ErrorHandler onErr);

  // Stop the current connection and close all sockets.
  void stop();

  // Should be called every frame. It initiates asynchronous read operations for all active connections.
  void poll();
};
