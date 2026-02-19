#include "GameRenderer/IChatRenderer.h"

#include "ChatRenderer.h"

std::unique_ptr<IChatRenderer> IChatRenderer::create() {
  return std::make_unique<ChatRenderer>();
}