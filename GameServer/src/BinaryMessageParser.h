#pragma once

#include <NetworkDataHandler/INetworkDataHandler.h>

#include <cstdint>
#include <vector>

#include "GameShared/GameMessageTypes.h"
#include "PerSocketData.h"

class BinaryMessageParser {
 public:
  // -------------
  // Signatures
  // -------------

  using OnBroadcastMessageCallback = std::function<void(INetworkDataHandler::MessageType type, const std::vector<uint8_t>&)>;
  using OnSendTypedBinaryToTheSocketCallback = std::function<void(GameMessageType type, WsType* ws, const std::vector<uint8_t>& payload)>;

  // --------------
  // Constructor
  // --------------

  BinaryMessageParser(
      OnBroadcastMessageCallback onBroadcastMessageCallback,
      OnSendTypedBinaryToTheSocketCallback onSendTypedBinaryToTheSocketCallback);

  // -----------
  // Interface
  // -----------

  void parseAnyBinaryMessage(
      INetworkDataHandler::MessageType type,
      const std::vector<uint8_t>& payload,
      WsType* ws,                           // from which socket we received this message
      PerSocketData* perSocketData) const;  // data assigned with this socket

 private:
  // --------------------
  // Private state
  // --------------------

  OnBroadcastMessageCallback onBroadcastMessageCallback_;
  OnSendTypedBinaryToTheSocketCallback onSendTypedBinaryToTheSocketCallback_;

  void on_GMT_ChatMessage(const std::vector<uint8_t>& payload, PerSocketData* perSocketData) const;
  void on_GMT_PingFromClient(const std::vector<uint8_t>& payload, WsType* ws, PerSocketData* perSocketData) const;
  void on_GMT_InputDataFromClient(const std::vector<uint8_t>& payload, WsType* ws, PerSocketData* perSocketData) const;
};
