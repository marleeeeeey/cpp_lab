#pragma once

#include "GameSharedObjects/Player.h"

// User data per socket
struct PerSocketData {
  Player player;
  std::string clientIp;
};

namespace uWS {
template <bool SSL, bool isServer, typename USERDATA>
struct WebSocket;  // forward declaration
}  // namespace uWS
using WsType = uWS::WebSocket<false, true, PerSocketData>;
