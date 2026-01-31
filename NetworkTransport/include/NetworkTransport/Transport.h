#pragma once
#include <functional>
#include <string>
#include <string_view>

struct ITransport {
  using OnOpen = std::function<void()>;
  using OnText = std::function<void(std::string_view)>;
  using OnClose = std::function<void(int code, std::string_view reason)>;
  using OnError = std::function<void(std::string_view)>;

  virtual ~ITransport() = default;

  virtual void connect(std::string url) = 0;
  virtual void sendText(std::string_view text) = 0;
  virtual void close() = 0;

  OnOpen onOpen;
  OnText onText;
  OnClose onClose;
  OnError onError;
};