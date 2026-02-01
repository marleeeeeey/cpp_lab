#pragma once

#include <string>
#include <vector>

class ChatDataForRendering {
 public:
  bool isConnected;

  void addMessage(const std::string& message);
  const std::vector<std::string>& getChatHistory() const;

 private:
  std::vector<std::string> chatHistory_;
};