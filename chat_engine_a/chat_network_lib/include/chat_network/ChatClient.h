#pragma once

#include <functional>
#include <memory>
#include <string>

class ChatClient {
  struct Impl;
  std::unique_ptr<Impl> pimpl;

 public:
  ChatClient();
  ~ChatClient();

 public:
  using OnReceiveMessageCallback = std::function<void(const std::string&)>;
  void start(const std::string& host, const std::string& port, const OnReceiveMessageCallback& callback);
  void stop();
  void poll();
  void send(const std::string& msg);
  bool isConnected() const;

 private:
  OnReceiveMessageCallback onReceiveMessageCallback;

 private:
  void doRead();
  void doWrite(std::shared_ptr<std::string> msgPtr);
};
