#include "chat_network/ChatClient.h"

#include <asio.hpp>

#define DISABLE_DEBUG_LOG
#include "debug_log/DebugLog.h"
#include "network_connection/NetworkConnection.h"

using asio::ip::tcp;

struct ChatClient::Impl {
  asio::io_context io_context;
  tcp::resolver resolver;
  std::unique_ptr<NetworkConnection> connection;  // Own the permanent socket
  tcp::socket tempSocket;                         // Temporary socket used for the connection process

  Impl() : tempSocket(io_context), resolver(io_context) {}

  ~Impl() {
    io_context.stop();
    std::cerr << "Client: Client Stopped" << std::endl;
  }
};

ChatClient::ChatClient() = default;

ChatClient::~ChatClient() = default;

void ChatClient::start(const std::string& host, short port,
                       MessageHandler onMsg, ErrorHandler onErr) {
  pimpl = std::make_unique<Impl>();
  asio::ip::basic_resolver_results<tcp> endpoints = pimpl->resolver.resolve(host, std::to_string(port));

  // Start connecting to the server
  asio::async_connect(pimpl->tempSocket, endpoints,
                      [this, onMsg = std::move(onMsg), onErr = std::move(onErr)](std::error_code ec, tcp::endpoint) {
                        if (!ec) {
                          std::cout << "Client: Connected to server!" << std::endl;
                          pimpl->connection = std::make_unique<NetworkConnection>(std::move(pimpl->tempSocket));
                          pimpl->connection->start(std::move(onMsg), std::move(onErr));
                        } else {
                          std::cerr << "Client: Connection failed: " << ec.message() << std::endl;
                        }
                      });

  std::cout << "Client: Client Started" << std::endl;
}

void ChatClient::stop() {
  pimpl.reset();
}

void ChatClient::poll() {
  if (!pimpl) {
    return;
  }
  pimpl->io_context.poll();
}

void ChatClient::send(const std::string& msg) {
  if (!isConnected()) {
    std::cerr << "Client: Not connected!" << std::endl;
    return;
  }
  pimpl->connection->send(msg);
}

bool ChatClient::isConnected() const {
  return pimpl && pimpl->connection && pimpl->connection->socket().is_open();
}
