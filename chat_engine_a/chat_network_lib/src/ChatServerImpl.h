#pragma once

#include <asio.hpp>
#include <memory>
#include <set>
#include <string>

// Forward declaration
class ClientSession;

// Represents a chat room for managing client sessions and broadcasting messages.
// This application has only one room.
class ChatRoom {
  std::set<std::shared_ptr<ClientSession>> sessions_;

 public:
  void join(std::shared_ptr<ClientSession> session);
  void leave(std::shared_ptr<ClientSession> session);
  void deliver(const std::string& msg);  // broadcasts the message to all sessions(clients)
};

// Represents a single client connection
class ClientSession : public std::enable_shared_from_this<ClientSession> {
  enum { max_length = 1024 };

  char data_[max_length];
  asio::ip::tcp::socket socket_;
  ChatRoom& room_;

 public:
  ClientSession(asio::ip::tcp::socket socket, ChatRoom& room);
  void start();  // Starts the asynchronous read operation
  void deliver(const std::string& msg);

 private:
  void read();
};
