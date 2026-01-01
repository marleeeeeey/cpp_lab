#include "ChatManager.h"

#include <iostream>
#include <ostream>

#include "chat_network/Handlers.h"
#include "debug_log/DebugLog.h"

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
}

const DataForRendering& ChatManager::getOutputDataForRendering() const {
  return dataForRendering;
}

void ChatManager::startServer() {
  chatServer.start(gServerPort);
}

void ChatManager::connectToServer() {
  MessageHandler onMsg = [this](const std::string& msg) {
    std::cout << "Client: Receive msg=" << msg << std::endl;
    dataForRendering.chatHistory.push_back(msg);
  };

  ErrorHandler onErr = [&](const std::string& msg) {
    std::cerr << msg << std::endl;
    // TODO Should we restart the connection here or do it in the logic above?
    chatClient.start(gServerAddress, gServerPort, onMsg, onErr);  // Reconnect on error.
  };

  chatClient.start(gServerAddress, gServerPort, onMsg, onErr);
}
