#pragma once
#include <functional>
#include <string_view>

struct ITransport {
  enum class SendResult { Success,
                          Error };

  using OnOpen = std::function<void()>;
  using OnText = std::function<void(std::string_view)>;
  using OnClose = std::function<void(int code, std::string_view reason)>;
  using OnError = std::function<void(std::string_view)>;

  virtual ~ITransport() = default;

  // Connect or reconnect to specific url (non-blocking).
  // Expected that connection thread (if needed) will be created here.
  virtual void connect(std::string_view url) = 0;

  // Gracefully close connection (blocking operation)
  virtual void close() = 0;

  // Send text and return a success flag
  virtual SendResult sendText(std::string_view text) = 0;

  OnOpen onOpen;
  OnText onText;
  OnClose onClose;
  OnError onError;
};