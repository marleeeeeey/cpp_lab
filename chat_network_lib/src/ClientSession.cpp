#include "ClientSession.h"

#include <iostream>

#include "ChatRoom.h"

using asio::ip::tcp;

ClientSession::ClientSession(tcp::socket socket, ChatRoom& room)
    : connection_(std::move(socket)), room_(room) {}

void ClientSession::start() {
  auto self = shared_from_this();

  // Start the connection
  MessageHandler onMsg = [this, self](std::span<const std::uint8_t> data) {
    std::string msg(reinterpret_cast<const char*>(data.data()), data.size());
    room_.deliver(msg);
  };
  ErrorHandler onErr = [this, self](const std::error_code& ec) {
    std::cerr << ec.message() << std::endl;
    room_.leave(shared_from_this());
  };
  connection_.start(onMsg, onErr);

  // Register the session in the room
  room_.join(self);
}

void ClientSession::deliver(const std::string& msg) {
  connection_.send(msg);
}
