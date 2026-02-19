#pragma once

#include <NetworkDataHandler/INetworkDataHandler.h>

#include <cstdint>
#include <vector>

#include "PerSocketData.h"

class BinaryMessageParser {
 public:
  // -------------
  // Signatures
  // -------------

  using OnBroadcastMessageCallback = std::function<void(INetworkDataHandler::MessageType type, const std::vector<uint8_t>&)>;

  // --------------
  // Constructor
  // --------------

  BinaryMessageParser(OnBroadcastMessageCallback onBroadcastMessageCallback);

  // -----------
  // Interface
  // -----------

  void parseAnyBinaryMessage(INetworkDataHandler::MessageType type, const std::vector<uint8_t>& payload, PerSocketData* perSocketData) const;

 private:
  // --------------------
  // Private state
  // --------------------

  OnBroadcastMessageCallback onBroadcastMessageCallback_;
  void on_GMT_ChatMessage(const std::vector<uint8_t>& payload, PerSocketData* perSocketData) const;
  void on_GMT_AnyPlayerDataUpdated(const std::vector<uint8_t>& payload, PerSocketData* perSocketData) const;
};
