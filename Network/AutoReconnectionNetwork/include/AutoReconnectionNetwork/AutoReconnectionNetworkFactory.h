#pragma once
#include <memory>

#include "IAutoReconnectionNetwork.h"

class AutoReconnectionNetworkFactory {
 public:
  static std::unique_ptr<IAutoReconnectionNetwork> create();
};