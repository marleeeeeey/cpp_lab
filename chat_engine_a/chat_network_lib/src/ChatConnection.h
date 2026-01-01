#pragma once

#include <asio.hpp>
#include <memory>
#include <string>

#include "chat_network/Handlers.h"

// Represents a single chat connection. It is a common class for both client and server.
// It sends/receives a size of the data first and then the data itself.
// It owns the underlying socket. In case of error, the socket is closed. ChatConnection can no longer be used.
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

  // Add a message to the outgoing queue and initiate an asynchronous write operation.
  void send(const std::string& msg);

  // Access the underlying socket. This object is only an owner of the socket.
  asio::ip::tcp::socket& socket();
};