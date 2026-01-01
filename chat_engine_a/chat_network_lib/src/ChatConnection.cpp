#include "ChatConnection.h"

#include <deque>

#define DISABLE_DEBUG_LOG
#include "debug_log/DebugLog.h"

struct OutgoingPackage {
  uint32_t size;
  std::vector<uint8_t> data;

  // 1. Handle raw byte spans
  explicit OutgoingPackage(std::span<const uint8_t> buffer)
      : size(static_cast<uint32_t>(buffer.size())),
        data(buffer.begin(), buffer.end()) {}

  // 2. Handle strings and string_views
  explicit OutgoingPackage(std::string_view msg)
      : size(static_cast<uint32_t>(msg.size())),
        data(reinterpret_cast<const uint8_t*>(msg.data()),
             reinterpret_cast<const uint8_t*>(msg.data()) + msg.size()) {}

  // 3. Handle direct std::string moves
  explicit OutgoingPackage(std::string&& msg)
      : size(static_cast<uint32_t>(msg.size())) {
    data.assign(reinterpret_cast<const uint8_t*>(msg.data()),
                reinterpret_cast<const uint8_t*>(msg.data()) + msg.size());
  }
};

struct ChatConnection::Impl {
  asio::ip::tcp::socket socket;
  uint32_t incomingSize = 0;
  std::vector<uint8_t> incomingData;
  std::deque<std::shared_ptr<OutgoingPackage>> writeQueue;
  MessageHandler onMessage;
  ErrorHandler onError;
  bool isConnected = true;  // Expected to be true for newly created connections

 public:  // Constructor
  Impl(asio::ip::tcp::socket socket) : socket(std::move(socket)) {}

 public:
  // Start the asynchronous read operation.
  // As a result, the onMessage callback will be called when a message is received.
  // If an error occurs, the onError callback will be called.
  void read() {
    // 1. Read the size of the incoming message
    asio::async_read(socket, asio::buffer(&incomingSize, sizeof(pimpl->incomingSize)),
                     [this](std::error_code ec, std::size_t) {
                       if (!ec) {
                         // 2. Read the incoming message with the specified size
                         incomingData.resize(incomingSize);
                         asio::async_read(socket, asio::buffer(incomingData),
                                          [this](std::error_code ec, std::size_t /*length*/) {
                                            if (!ec) {
                                              std::span<const std::uint8_t> data = incomingData;
                                              if (onMessage) onMessage(data);
                                              read();  // 3. Wait for the next message
                                            }
                                          });
                       } else {
                         std::cerr << "ChatClient: Read failed: " << ec.message() << std::endl;
                         stop();
                         if (onError) onError(ec);
                       }
                     });
  }

  // Send the next message in the queue. If there are no more messages, do nothing.
  void write() {
    if (writeQueue.empty()) return;

    auto& package = writeQueue.front();

    std::vector<asio::const_buffer> buffers;
    buffers.push_back(asio::buffer(&package->size, sizeof(package->size)));
    buffers.push_back(asio::buffer(package->data));

    asio::async_write(socket, buffers,
                      [package, this]  // Extent lifetime of the package
                      (std::error_code ec, std::size_t bytes_transferred) {
                        if (!ec) {
                          writeQueue.pop_front();  // Remove the package from the queue
                          debugLog() << "ChatClient::send success: sent " << bytes_transferred << " bytes." << std::endl;

                          // If there are more packages in the queue, send them
                          if (!writeQueue.empty()) {
                            write();
                          }
                        } else {
                          std::cerr << "ChatClient: Async write failed: " << ec.message() << std::endl;
                          stop();
                          if (onError) onError(ec);
                        }
                      });
  }

  void stop() {
    if (!socket.is_open()) {
      return;
    }

    asio::error_code ec;
    socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);  // Gracefully disable send/receive operations at the protocol level
    socket.close(ec);                                           // Close the socket. This triggers 'operation_aborted' in all pending async handlers.
    writeQueue.clear();                                         // Clear the queue as messages can no longer be delivered
    isConnected = false;

    std::cerr << "ChatClient: Connection closed." << std::endl;
  }
};

ChatConnection::ChatConnection(asio::ip::tcp::socket socket)
    : pimpl(std::make_unique<Impl>(std::move(socket))) {
}

ChatConnection::~ChatConnection() {
  pimpl->stop();
}

void ChatConnection::start(MessageHandler onMsg, ErrorHandler onErr) {
  assert(pimpl->isConnected);

  pimpl->onMessage = std::move(onMsg);
  pimpl->onError = std::move(onErr);
  pimpl->read();
}

void ChatConnection::send(std::span<const std::uint8_t> data) {
  assert(pimpl->isConnected);

  bool writeInProgress = !pimpl->writeQueue.empty();

  // Create a copy of the message in the heap with shared ownership.
  pimpl->writeQueue.push_back(std::make_shared<OutgoingPackage>(data));

  if (!writeInProgress) {
    pimpl->write();
  }
}

void ChatConnection::send(std::string_view msg) {
  send(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(msg.data()), msg.size()));
}

asio::ip::tcp::socket& ChatConnection::socket() {
  return pimpl->socket;
}
