#include "chat_network/ChatClient.h"

#include <asio.hpp>

#define DISABLE_DEBUG_LOG
#include "debug_log/DebugLog.h"

using asio::ip::tcp;

struct ChatClient::Impl {
  asio::io_context io_context;
  tcp::socket socket;
  tcp::resolver resolver;
  bool isConnected = false;
  uint32_t incoming_size;
  std::vector<uint8_t> incoming_data;

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
                          read();  // Start listening for messages
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

bool ChatClient::isConnected() const {
  return pimpl && pimpl->socket.is_open() && pimpl->isConnected;
}

struct OutgoingPackage {
  uint32_t size;
  std::string data;

  explicit OutgoingPackage(std::string msg)
      : size(static_cast<uint32_t>(msg.size())), data(std::move(msg)) {}
};

void ChatClient::send(const std::string& msg) {
  if (!pimpl) {
    std::cerr << "ChatClient::send: Method ChatClient::start must be called first." << std::endl;
    return;
  }

  debugLog() << "ChatClient::write: " << msg << std::endl;

  // Create a copy of the message in the heap with shared ownership.
  auto package = std::make_shared<OutgoingPackage>(msg);

  std::vector<asio::const_buffer> buffers;
  buffers.push_back(asio::buffer(&package->size, sizeof(package->size)));
  buffers.push_back(asio::buffer(package->data));

  asio::async_write(pimpl->socket, buffers,
                    [package, this]  // Extent lifetime of the package
                    (std::error_code ec, std::size_t bytes_transferred) {
                      if (!ec) {
                        debugLog() << "ChatClient::send success: sent " << bytes_transferred << " bytes." << std::endl;
                      } else {
                        std::cerr << "ChatClient: Async write failed: " << ec.message() << std::endl;
                        stop();  // Stop the connection on error
                      }
                    });
}

void ChatClient::read() {
  assert(pimpl);

  // 1. Read the size of the incoming message
  asio::async_read(pimpl->socket, asio::buffer(&pimpl->incoming_size, sizeof(pimpl->incoming_size)),
                   [this](std::error_code ec, std::size_t) {
                     if (!ec) {
                       // 2. Read the incoming message with the specified size
                       pimpl->incoming_data.resize(pimpl->incoming_size);
                       asio::async_read(pimpl->socket, asio::buffer(pimpl->incoming_data),
                                        [this](std::error_code ec, std::size_t /*length*/) {
                                          if (!ec) {
                                            std::string msg(pimpl->incoming_data.begin(), pimpl->incoming_data.end());
                                            if (onReceiveMessageCallback) onReceiveMessageCallback(msg);
                                            debugLog() << "ChatClient: Received: " << msg << std::endl;
                                            read();  // 3. Wait for the next message
                                          }
                                        });
                     } else {
                       std::cerr << "ChatClient: Read failed: " << ec.message() << std::endl;
                       stop();  // If error stop the connection
                     }
                   });
}
