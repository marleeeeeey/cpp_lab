#include "DoubleQueueNetwork.h"
#include "DoubleQueueNetwork/DoubleQueueNetworkFactory.h"

std::unique_ptr<IDoubleQueueNetwork> DoubleQueueNetworkFactory::createDoubleQueueNetwork() {
  return std::make_unique<DoubleQueueNetwork>();
}