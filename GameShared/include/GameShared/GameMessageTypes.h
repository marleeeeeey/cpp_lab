#pragma once

// Avoid "enum class" to allow implicit conversions
enum GameMessageType {
  GMT_ChatMessage = 3333,
  GMT_NumberOfClients = 3334,
  GMT_PlayerIdFromServer = 3335,
  GMT_PingFromClient = 3336,
  GMT_InputDataFromClient = 3337,
  GMT_WorldSnapshot = 3338,
};