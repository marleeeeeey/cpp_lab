#include "NetworkDataHandler.h"

std::unique_ptr<INetworkDataHandler> INetworkDataHandler::create() {
  return std::make_unique<NetworkDataHandler>();
}