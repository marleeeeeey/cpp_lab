#pragma once

#include <memory>

#include "IDoubleQueueNetwork.h"

class DoubleQueueNetworkFactory {
 public:
  static std::unique_ptr<IDoubleQueueNetwork> createDoubleQueueNetwork();
};
