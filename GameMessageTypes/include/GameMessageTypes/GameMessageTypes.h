#pragma once

// Avoid "enum class" to allow implicit conversions
enum GameMessageType {
  GMT_TextMessage = 3333,
  GMT_NumberOfClients = 3334
};