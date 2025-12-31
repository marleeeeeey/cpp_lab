#pragma once

#include <memory>

// Creates one room to place all new clients (connections) to this room.
// USAGE:
// ChatServer server;
// server.init(port);
// server.poll();
class ChatServer {
  struct Impl;
  std::unique_ptr<Impl> pimpl;

 public:
  // Ctor/Dtor must be declared here and defined in the cpp file when using unique_ptr.
  // Because unique_ptr should know the size of the object.
  ChatServer();
  ~ChatServer();

 public:
  void start(short port);  // Call once at the beginning of the program.
  void stop();
  void poll();  // Call every frame.
};
