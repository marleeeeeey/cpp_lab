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
    : socket_(std::move(socket)), room_(room) {}

void ClientSession::start() {
  room_.join(shared_from_this());
  read();
}

void ClientSession::deliver(const std::string& msg) {
  auto self(shared_from_this());
  auto msgCopy = std::make_shared<std::string>(msg);

  asio::async_write(socket_, asio::buffer(*msgCopy),
                    [self, msgCopy]  // Extent lifetime for self and asio::buffer
                    (std::error_code ec, std::size_t /*length*/) {
                      // If error occurs, the session will eventually be dropped by the read loop
                    });
}

void ClientSession::read() {
  auto self(shared_from_this());
  socket_.async_read_some(asio::buffer(data_, max_length),
                          [this, self]  // Extent lifetime for self
                          (std::error_code ec, std::size_t length) {
                            if (!ec) {
                              std::string msg(data_, length);
                              room_.deliver(msg);  // Send it to everyone
                              read();              // Wait for the next message
                            } else {
                              room_.leave(shared_from_this());
                            }
                          });
}
