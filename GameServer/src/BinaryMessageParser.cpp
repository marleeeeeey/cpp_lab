#include "BinaryMessageParser.h"

#include <GameMessageTypes/GameMessageTypes.h>
#include <GameSerialization/GameSerialization.h>
#include <GameSharedObjects/ChatMessage.h>
#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

BinaryMessageParser::BinaryMessageParser(
    OnBroadcastMessageCallback onBroadcastMessageCallback,
    OnSendTypedBinaryToTheSocketCallback onSendTypedBinaryToTheSocketCallback) {
  onBroadcastMessageCallback_ = onBroadcastMessageCallback;
  onSendTypedBinaryToTheSocketCallback_ = onSendTypedBinaryToTheSocketCallback;
  assert(onBroadcastMessageCallback_);
  assert(onSendTypedBinaryToTheSocketCallback_);
}

void BinaryMessageParser::parseAnyBinaryMessage(
    INetworkDataHandler::MessageType type,
    const std::vector<uint8_t>& payload,
    WsType* ws,
    PerSocketData* perSocketData) const {
  switch (type) {
    case GMT_ChatMessage:
      on_GMT_ChatMessage(payload, perSocketData);
      break;
    case GMT_AnyPlayerDataUpdated:
      on_GMT_AnyPlayerDataUpdated(payload, perSocketData);
      break;
    case GMT_PingFromClient:
      on_GMT_PingFromClient(payload, ws, perSocketData);
      break;
    default:
      SPDLOG_ERROR("Unknown message type: ()", type);
  }
}

void BinaryMessageParser::on_GMT_ChatMessage(const std::vector<uint8_t>& payload, PerSocketData* perSocketData) const {
  ChatMessage chatMessage = GameSerialization::deserializeChatMessage(payload);
  chatMessage.sender = perSocketData->player;  // Update sender data. Keep timestamp data and msg the same.
  auto replyPayload = GameSerialization::serializeChatMessage(chatMessage);
  onBroadcastMessageCallback_(GMT_ChatMessage, replyPayload);
  SPDLOG_INFO("{}: {}", chatMessage.sender.name, chatMessage.message);
}

void BinaryMessageParser::on_GMT_AnyPlayerDataUpdated(const std::vector<uint8_t>& payload, PerSocketData* perSocketData) const {
  onBroadcastMessageCallback_(GMT_AnyPlayerDataUpdated, payload);  // just transfer data as is
}

void BinaryMessageParser::on_GMT_PingFromClient(const std::vector<uint8_t>& payload, WsType* ws, PerSocketData* perSocketData) const {
  onSendTypedBinaryToTheSocketCallback_(GMT_PingFromClient, ws, payload);
}
