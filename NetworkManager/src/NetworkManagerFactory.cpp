#include "NetworkManager/NetworkManagerFactory.h"

#include "NetworkManager.h"

std::unique_ptr<INetworkManager> NetworkManagerFactory::createNetworkManager() {
  return std::make_unique<NetworkManager>(
      NetworkManager::NetworkOptions{
          .useOutboundQueue = true});
}