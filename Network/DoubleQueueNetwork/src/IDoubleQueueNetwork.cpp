#include "DoubleQueueNetwork/IDoubleQueueNetwork.h"

#include "DoubleQueueNetwork.h"

std::unique_ptr<IDoubleQueueNetwork> IDoubleQueueNetwork::create() {
  return std::make_unique<DoubleQueueNetwork>();
}