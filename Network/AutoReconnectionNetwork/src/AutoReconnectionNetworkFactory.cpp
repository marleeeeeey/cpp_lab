#include "AutoReconnectionNetwork/AutoReconnectionNetworkFactory.h"

#include <memory>

#include "AutoReconnectionNetwork.h"

std::unique_ptr<IAutoReconnectionNetwork> AutoReconnectionNetworkFactory::create() {
  return std::make_unique<AutoReconnectionNetwork>();
}