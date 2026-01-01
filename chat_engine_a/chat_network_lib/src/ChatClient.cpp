#include "chat_network/ChatClient.h"

#include <asio.hpp>

#define DISABLE_DEBUG_LOG
#include "debug_log/DebugLog.h"

using asio::ip::tcp;

struct ChatClient::Impl {
  asio::io_context io_context;
  tcp::socket socket;
  tcp::resolver resolver;
  char read_msg[1024];
  bool isConnected = false;

  Impl() : socket(io_context), resolver(io_context) {}
};

ChatClient::ChatClient() = default;

ChatClient::~ChatClient() = default;

void ChatClient::start(const std::string& host, short port, const OnReceiveMessageCallback& callback) {
  pimpl = std::make_unique<Impl>();
  asio::ip::basic_resolver_results<tcp> endpoints = pimpl->resolver.resolve(host, std::to_string(port));

  onReceiveMessageCallback = callback;

  // Start connecting to the server
  asio::async_connect(pimpl->socket, endpoints,
                      [this](std::error_code ec, tcp::endpoint) {
                        if (!ec) {
                          std::cout << "Client: Connected to server!" << std::endl;
                          pimpl->isConnected = true;
                          doRead();  // Start listening for messages
                        } else {
                          std::cerr << "Client: Connection failed: " << ec.message() << std::endl;
                          pimpl->isConnected = false;
                        }
                      });
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
  if (!pimpl) {
    std::cerr << "ChatClient::send: Method ChatClient::start must be called first." << std::endl;
    return;
  }

  // IMPORTANT: This method is called from the main thread
  debugLog() << "ChatClient::write: " << msg << std::endl;

  // Create a copy of the message in the heap with shared ownership.
  auto msgPtr = std::make_shared<std::string>(msg);

  // Post the write to the io_context thread to ensure thread safety
  asio::post(pimpl->io_context, [this, msgPtr]  // IMPORTANT: Extend lifetime of msgPtr by capturing it in lambda.
             () {
               // IMPORTANT: This method is called from the io_context thread
               debugLog() << "ChatClient::write: asio::post" << std::endl;
               doWrite(msgPtr);  // IMPORTANT: Extend lifetime of msgPtr.
             });
}

bool ChatClient::isConnected() const {
  return pimpl && pimpl->socket.is_open() && pimpl->isConnected;
}

void ChatClient::doRead() {
  assert(pimpl);
  pimpl->socket.async_read_some(asio::buffer(pimpl->read_msg),
                                [this](std::error_code ec, std::size_t length) {
                                  if (!ec) {
                                    // Print received message to console
                                    auto msg = std::string(pimpl->read_msg, length);
                                    onReceiveMessageCallback(msg);
                                    debugLog() << "ChatClient: Received: " << msg << std::endl;
                                    doRead();  // Wait for more data
                                  } else {
                                    std::cout << "ChatClient: Disconnected from server." << std::endl;
                                    pimpl->socket.close();
                                  }
                                });
}

void ChatClient::doWrite(std::shared_ptr<std::string> msgPtr) {
  assert(pimpl);
  debugLog() << "ChatClient::do_write: msg=" << *msgPtr << std::endl;
  // IMPORTANT: This method is called from the io_context thread
  asio::async_write(pimpl->socket, asio::buffer(*msgPtr),
                    [msgPtr]  // IMPORTANT: Extend the lifetime of msgPtr by capturing it in lambda.
                              // Otherwise, it will be deleted and asio::buffer becomes invalid.
                    (std::error_code ec, std::size_t) {
                      // IMPORTANT: This method is called from the io_context thread
                      debugLog() << "ChatClient::do_write: asio::async_write completed. Callback invoked." << std::endl;
                      if (ec) {
                        std::cerr << "ChatClient: Write failed: " << ec.message() << std::endl;
                      }
                      // IMPORTANT: msgPtr goes out of scope here, and the string is finally deleted
                    });
}