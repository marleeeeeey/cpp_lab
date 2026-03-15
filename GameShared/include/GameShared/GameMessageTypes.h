#pragma once

// Avoid "enum class" to allow implicit conversions
// "GMT" stands for "Game Message Type"
enum GameMessageType {
  // ------------------------------------------------
  // Both Directions ( Client -> Server -> Client )
  // ------------------------------------------------

  GMT_ChatMessage = 3333,
  GMT_PingFromClient = 3336,

  // ------------------
  // Client -> Server
  // ------------------

  GMT_InputDataFromClient = 3337,

  // ------------------
  // Server -> Client
  // ------------------

  GMT_NumberOfClientsFromServer = 3334,
  GMT_PlayerIdFromServer = 3335,
  GMT_WorldSnapshotFromServer = 3338,
};