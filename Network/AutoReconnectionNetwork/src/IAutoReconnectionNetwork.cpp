#include "AutoReconnectionNetwork/IAutoReconnectionNetwork.h"

#include "AutoReconnectionNetwork.h"

std::unique_ptr<IAutoReconnectionNetwork> IAutoReconnectionNetwork::create() {
  return std::make_unique<AutoReconnectionNetwork>();
}