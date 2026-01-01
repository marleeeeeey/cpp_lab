#include "ChatManager.h"

#include <iostream>
#include <ostream>

void ChatManager::init(uint8_t modeMask) {
  dataForRendering.chatHistory.push_back("Hello world!");
  dataForRendering.isConnected = true;

  if (modeMask & Mode::Server) {
    chatServer.start(12345);
  }

  if (modeMask & Mode::Client) {
    chatClient.start("127.0.0.1", "12345", [this](const std::string& msg) {
      std::cout << "Received message from server: " << msg << std::endl;
      dataForRendering.chatHistory.push_back(msg);
    });
  }
}

void ChatManager::iterate(double elapsed) {
  chatServer.poll();
  chatClient.poll();
}

void ChatManager::stop() {
  chatServer.stop();
  chatClient.stop();
}

void ChatManager::sendMessage(const std::string& msg) {
  chatClient.send(msg);
  // Not needed to add a message to chat history, because it returns from the server.
  // dataForRendering.chatHistory.push_back(msg);
}

const DataForRendering& ChatManager::getOutputDataForRendering() const {
  return dataForRendering;
}
