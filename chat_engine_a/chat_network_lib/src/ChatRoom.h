#pragma once

#include <asio.hpp>
#include <memory>
#include <set>
#include <string>

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
