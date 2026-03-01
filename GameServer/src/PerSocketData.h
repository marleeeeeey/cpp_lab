#pragma once

#include "GameShared/Player.h"

// -------------------------
// WebSocket type (WsType)
// -------------------------

struct PerSocketData;
namespace uWS {
template <bool SSL, bool isServer, typename USERDATA>
struct WebSocket;  // forward declaration
}  // namespace uWS
using WsType = uWS::WebSocket<false, true, PerSocketData>;  // <<<<

// -----------------------------
// User data per socket
// -----------------------------

struct PerSocketData {
  Player player;
  std::string clientIp;
  WsType* ws;  // uses to reply to this socket only
};
