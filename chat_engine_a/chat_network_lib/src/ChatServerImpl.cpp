#include "ChatServerImpl.h"

#include <iostream>

#define DISABLE_DEBUG_LOG
#include "debug_log/DebugLog.h"

using asio::ip::tcp;

void ChatRoom::join(std::shared_ptr<ClientSession> session) {
  sessions_.insert(session);
  std::cout << "Server: Client joined. Total clients: " << sessions_.size() << std::endl;
}

void ChatRoom::leave(std::shared_ptr<ClientSession> session) {
  sessions_.erase(session);
  std::cout << "ChatRoom::leave: Client left. Total clients: " << sessions_.size() << std::endl;
}

void ChatRoom::deliver(const std::string& msg) {
  debugLog() << "ChatRoom::deliver: msg= " << msg << std::endl;
  for (auto& session : sessions_) {
    session->deliver(msg);
  }
}

ClientSession::ClientSession(tcp::socket socket, ChatRoom& room)
    : connection_(std::move(socket)), room_(room) {}

void ClientSession::start() {
  auto self = shared_from_this();

  // Start the connection
  MessageHandler onMsg = [this, self](const std::string& msg) {
    room_.deliver(msg);
  };
  ErrorHandler onErr = [this, self](const std::string& msg) {
    std::cerr << msg << std::endl;
    room_.leave(shared_from_this());
  };
  connection_.start(onMsg, onErr);

  // Register the session in the room
  room_.join(self);
}

void ClientSession::deliver(const std::string& msg) {
  connection_.send(msg);
}
