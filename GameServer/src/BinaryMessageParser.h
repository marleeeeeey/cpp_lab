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

  using OnBroadcastMessageCallback = std::function<void(PayloadType type, PayloadView)>;
  using OnSendTypedBinaryToTheSocketCallback = std::function<void(GameMessageType type, WsType* ws, PayloadView payload)>;

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
      PayloadType type,
      PayloadView payload,
      // from which socket we received this message
      PerSocketData* perSocketData) const;  // data assigned with this socket

 private:
  // --------------------
  // Private state
  // --------------------

  OnBroadcastMessageCallback onBroadcastMessageCallback_;
  OnSendTypedBinaryToTheSocketCallback onSendTypedBinaryToTheSocketCallback_;

  void on_GMT_ChatMessage(PayloadView payload, PerSocketData* perSocketData) const;
  void on_GMT_PingFromClient(PayloadView payload, PerSocketData* perSocketData) const;
  void on_GMT_InputDataFromClient(PayloadView payload, PerSocketData* perSocketData) const;
};
