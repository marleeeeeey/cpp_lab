#pragma once

#include <memory>

#include "INetworkManager.h"

class NetworkManagerFactory {
 public:
  static std::unique_ptr<INetworkManager> createNetworkManager();
};
