#pragma once

#include <string>
#include <vector>

class ChatDataForRendering {
 public:
  std::string connectionStatus;
  std::string numberOfConnectedUsers{"0"};

  void addMessage(const std::string& message);
  const std::vector<std::string>& getChatHistory() const;

 private:
  std::vector<std::string> chatHistory_;
};