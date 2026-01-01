#include "ChatRoom.h"

#include <iostream>

#include "ClientSession.h"

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
