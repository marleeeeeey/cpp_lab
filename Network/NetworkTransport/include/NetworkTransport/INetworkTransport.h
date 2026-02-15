#pragma once
#include <functional>
#include <memory>
#include <string_view>

class INetworkTransport {
 public:
  // ------------
  // Factory
  // ------------

  static std::unique_ptr<INetworkTransport> create();
  virtual ~INetworkTransport() = default;

  // ------------
  // Signatures
  // ------------

  enum class SendResult { Success,
                          Error };

  using OnOpen = std::function<void()>;
  using OnText = std::function<void(std::string_view)>;
  using OnBinary = std::function<void(uint8_t* data, int size)>;
  using OnClose = std::function<void(int code, std::string_view reason)>;
  using OnError = std::function<void(std::string_view)>;

  // ------------
  // Interface
  // ------------

  // Connect or reconnect to specific url (non-blocking).
  // Expected that connection thread (if needed) will be created here.
  virtual void connect(std::string_view url) = 0;

  // Gracefully close connection (blocking operation)
  virtual void close() = 0;

  // Send text and return a success flag
  virtual SendResult sendText(std::string_view text) = 0;

  // Send binary data and return a success flag
  // The assigned std::vector must be kept alive for the lifetime of the input buffer
  virtual SendResult sendBinary(const std::vector<uint8_t>& data) = 0;

  OnOpen onOpen;
  OnText onText;
  OnBinary onBinary;
  OnClose onClose;
  OnError onError;
};