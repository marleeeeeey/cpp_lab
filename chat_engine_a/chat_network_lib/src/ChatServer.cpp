#include "chat_network/ChatServer.h"

#include <iostream>
#include <string>

#include "ChatServerImpl.h"

#define DISABLE_DEBUG_LOG
#include "debug_log/DebugLog.h"

using asio::ip::tcp;

struct ChatServer::Impl {
  asio::io_context io_context;
  tcp::acceptor acceptor;
  ChatRoom room;

  explicit Impl(short port) : acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}

  ~Impl() { io_context.stop(); }

  void accept() {
    acceptor.async_accept(
        [this](std::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::cout << "Server: Accepted new connection " << socket.remote_endpoint() << std::endl;
            std::make_shared<ClientSession>(std::move(socket), room)->start();
          }
          accept();  // Wait for the next connection
        });
  }
};

ChatServer::ChatServer() = default;

ChatServer::~ChatServer() = default;

void ChatServer::start(short port) {
  debugLog() << "ChatServer::start: port=" << port << std::endl;
  pimpl = std::make_unique<Impl>(port);
  pimpl->accept();
}

void ChatServer::stop() {
  pimpl.reset();
  debugLog() << "ChatServer::stop: OK" << std::endl;
}

void ChatServer::poll() {
  if (!pimpl) {
    return;
  }
  pimpl->io_context.poll();
}
