#include "ChatManager.h"

void ChatManager::init() {
  // TODO
  dataForRendering.chatHistory.push_back("Hello world!");
  dataForRendering.chatHistory.push_back("How are you?");
  dataForRendering.chatHistory.push_back("I'm fine, thanks!");
  dataForRendering.isConnected = true;
}

void ChatManager::iterate(double elapsed) {
  // TODO
}

const DataForRendering& ChatManager::getOutputDataForRendering() const {
  return dataForRendering;
}
