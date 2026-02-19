#pragma once

// Avoid "enum class" to allow implicit conversions
enum GameMessageType {
  GMT_ChatMessage = 3333,
  GMT_NumberOfClients,
  GMT_AnyPlayerDataUpdated,
  GMT_AssignNameToThePlayer,
};