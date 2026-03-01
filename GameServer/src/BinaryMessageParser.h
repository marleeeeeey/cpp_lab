#pragma once

#include "PerSocketData.h"

class BinaryMessageParser {
 public:
  BinaryMessageParser(const BroadcastCb& broadcastCb, const SendToSocketCb& sendToSocketCb);
  void parseAnyBinaryMessage(PayloadType type, PayloadView payload, PerSocketData* perSocketData) const;

 private:
  BroadcastCb broadcastCb_;
  SendToSocketCb sendToSocketCb_;

  void on_GMT_ChatMessage(PayloadView payload, PerSocketData* perSocketData) const;
  void on_GMT_PingFromClient(PayloadView payload, PerSocketData* perSocketData) const;
  void on_GMT_InputDataFromClient(PayloadView payload, PerSocketData* perSocketData) const;
};
