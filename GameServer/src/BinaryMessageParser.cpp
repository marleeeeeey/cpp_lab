#include "BinaryMessageParser.h"

#include <GameSerialization/GameSerialization.h>
#include <GameShared/ChatMessage.h>
#include <GameShared/GameMessageTypes.h>
#include <GameShared/InputPacket.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <magic_enum/magic_enum.hpp>

BinaryMessageParser::BinaryMessageParser(
    const BroadcastCb& broadcastCb,
    const SendToSocketCb& sendToSocketCb) {
  broadcastCb_ = broadcastCb;
  sendToSocketCb_ = sendToSocketCb;
}

void BinaryMessageParser::parseAnyBinaryMessage(
    PayloadType type,
    PayloadView payload,
    PerSocketData* perSocketData) const {
  switch (type) {
    case GMT_ChatMessage:
      on_GMT_ChatMessage(payload, perSocketData);
      break;
    case GMT_PingFromClient:
      on_GMT_PingFromClient(payload, perSocketData);
      break;
    case GMT_InputDataFromClient:
      on_GMT_InputDataFromClient(payload, perSocketData);
      break;
    default:
      SPDLOG_ERROR("Unknown message type: ()", type);
  }
}

void BinaryMessageParser::on_GMT_ChatMessage(PayloadView payload, PerSocketData* perSocketData) const {
  ChatMessage chatMessage = GameSerialization::deserializeChatMessage(payload);
  chatMessage.sender = perSocketData->player;  // Update sender data. Keep timestamp data and msg the same.
  auto replyPayload = GameSerialization::serializeChatMessage(chatMessage);
  broadcastCb_(GMT_ChatMessage, replyPayload);
  SPDLOG_INFO("{}: {}", chatMessage.sender.name, chatMessage.message);
}

void BinaryMessageParser::on_GMT_PingFromClient(PayloadView payload, PerSocketData* perSocketData) const {
  sendToSocketCb_(GMT_PingFromClient, perSocketData->ws, payload);
}

void BinaryMessageParser::on_GMT_InputDataFromClient(PayloadView payload, PerSocketData* perSocketData) const {
  auto inputPacket = GameSerialization::deserializeInputPacket(payload);
  auto& player = perSocketData->player;
  player.lastInput.move.x = std::clamp(inputPacket.moveX, -1.0f, 1.0f);
  player.lastInput.move.y = std::clamp(inputPacket.moveY, -1.0f, 1.0f);
}
