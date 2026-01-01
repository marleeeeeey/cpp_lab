#pragma once

#include <asio.hpp>
#include <memory>
#include <string>

#include "../include/chat_network/Handlers.h"

class ChatConnection : public std::enable_shared_from_this<ChatConnection> {
  struct Impl;
  std::unique_ptr<Impl> pimpl;

 public:
  explicit ChatConnection(asio::ip::tcp::socket socket);
  ~ChatConnection();

  // Start the asynchronous operation of reading a message.
  // The onMessage callback will be called when a message is received.
  // If an error occurs, the onError callback will be called. Socks will be closed. This connection can no longer be used.
  void start(MessageHandler onMsg, ErrorHandler onErr);
  void send(const std::string& msg);
  asio::ip::tcp::socket& socket();  // ChatConnection is only an owner of the socket.
};