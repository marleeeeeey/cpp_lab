#include "ChatManager.h"

void ChatManager::init() {
  dataForRendering.chatHistory.push_back("Hello world!");
  dataForRendering.isConnected = true;

  chatServer.start(12345);
}

void ChatManager::iterate(double elapsed) {
  chatServer.poll();
}

void ChatManager::stop() {
  chatServer.stop();
}

const DataForRendering& ChatManager::getOutputDataForRendering() const {
  return dataForRendering;
}
