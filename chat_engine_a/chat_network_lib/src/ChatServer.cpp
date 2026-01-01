#include "chat_network/ChatServer.h"

#include <iostream>
#include <string>

#include "ChatRoom.h"
#include "ClientSession.h"

#define DISABLE_DEBUG_LOG
#include "debug_log/DebugLog.h"

using asio::ip::tcp;

struct ChatServer::Impl {
  asio::io_context io_context;
  tcp::acceptor acceptor;
  ChatRoom room;
  ErrorHandler onErr;

  // Change: Constructor no longer binds the socket immediately
  explicit Impl() : acceptor(io_context) {}

  ~Impl() {
    io_context.stop();
    std::cerr << "Server: Server Stopped" << std::endl;
  }

  void start(short port, ErrorHandler onErr_) {
    onErr = std::move(onErr_);

    asio::error_code ec;
    tcp::endpoint endpoint(tcp::v4(), port);

    acceptor.open(endpoint.protocol(), ec);
    // Explicitly disable address reuse to have an error if the port is already in use
    acceptor.set_option(tcp::acceptor::reuse_address(false), ec);
    acceptor.bind(endpoint, ec);
    if (ec) {
      std::cerr << "Server Error: Port " << port << " is already in use! Message: " << ec.message() << std::endl;
      onErr(ec);
      return;
    }

    acceptor.listen(asio::socket_base::max_listen_connections, ec);
    accept();

    std::cout << "Server: Server Started" << std::endl;
  }

 private:
  void accept() {
    acceptor.async_accept(
        [this](std::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::cout << "Server: Accepted new connection " << socket.remote_endpoint() << std::endl;
            std::make_shared<ClientSession>(std::move(socket), room)->start();
          } else {
            std::cerr << "Server: Accept failed: " << ec.message() << std::endl;
          }
          accept();  // Wait for the next connection
        });
  }
};

ChatServer::ChatServer() = default;

ChatServer::~ChatServer() = default;

void ChatServer::start(short port, ErrorHandler onErr) {
  debugLog() << "ChatServer::start: port=" << port << std::endl;
  pimpl = std::make_unique<Impl>();
  pimpl->start(port, std::move(onErr));
}

void ChatServer::stop() {
  pimpl.reset();
}

void ChatServer::poll() {
  if (!pimpl) {
    return;
  }
  pimpl->io_context.poll();
}
