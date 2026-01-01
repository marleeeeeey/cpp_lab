#include "ChatManager.h"

#include <iostream>
#include <ostream>

namespace {
std::string gServerAddress = "127.0.0.1";
short gServerPort = 12345;
}  // namespace

void ChatManager::init(uint8_t modeMask) {
  modeMask_ = modeMask;

  if (modeMask & Mode::Server) {
    startServer();
  }

  if (modeMask & Mode::Client) {
    connectToServer();
  }
}

void ChatManager::iterate(double elapsed) {
  chatServer.poll();
  chatClient.poll();

  if (modeMask_ & Mode::Client) {
    bool isConnected = chatClient.isConnected();
    dataForRendering.isConnected = isConnected;

    if (!isConnected) {
      connectToServer();  // reconnect if the connection was lost
    }
  }
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

void ChatManager::startServer() {
  chatServer.start(gServerPort);
}

void ChatManager::connectToServer() {
  chatClient.start(gServerAddress, gServerPort, [this](const std::string& msg) {
    std::cout << "Received message from server: " << msg << std::endl;
    dataForRendering.chatHistory.push_back(msg);
  });
}
