#pragma once

#include <asio.hpp>
#include <memory>
#include <string>

#include "ChatConnection.h"

class ChatRoom;

// Represents a single client connection
class ClientSession : public std::enable_shared_from_this<ClientSession> {
  ChatConnection connection_;
  ChatRoom& room_;

 public:
  ClientSession(asio::ip::tcp::socket socket, ChatRoom& room);
  void start();  // Starts the asynchronous read operation
  void deliver(const std::string& msg);

 private:
  void read();
};
