#include "GameRenderer/IDebugRender.h"

#include "DebugRender.h"

std::unique_ptr<IDebugRender> IDebugRender::create() {
  return std::make_unique<DebugRender>();
}