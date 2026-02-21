#pragma once
#include <memory>

#include "AutoReconnectionNetwork/IAutoReconnectionNetwork.h"
#include "GameRenderer/IChatRenderer.h"
#include "GameRenderer/IGameWorldRenderer.h"
#include "GameSharedObjects/ChatMessage.h"
#include "GameSharedObjects/Player.h"
#include "GameTimer.h"
#include "GameWorld.h"
#include "NetworkDataHandler/INetworkDataHandler.h"

class GameNetwork {
 public:
  GameNetwork(
      const std::string& url,
      std::weak_ptr<IChatRenderer> chatRenderer,
      std::weak_ptr<IGameWorldRenderer> gameWorldRenderer,
      std::weak_ptr<GameWorld> gameWorld,
      std::weak_ptr<GameTimer> gameTimer);

  // ------------------------
  // Connection Interface
  // ------------------------

  void start();
  void iterate();
  void stop();

  // --------------------------
  // Transfer Data Interface
  // --------------------------

  void sendPlayer(const Player& player);
  void sendChatMessage(const ChatMessage& message);
  void sendPingFromClient();

 private:
  // ----------------------------
  // Init (Private Methods)
  // ----------------------------

  void initNetworkDataHandlers_();
  void initAutoReconnectionNetwork_();

  // ---------------------
  // Ownership data
  // ---------------------

  std::unique_ptr<IAutoReconnectionNetwork> autoReconnectionNetwork_;
  std::unique_ptr<INetworkDataHandler> networkDataHandler_;
  std::string url_;

  // ---------------------
  // Shared data
  // ---------------------

  std::weak_ptr<IChatRenderer> chatRenderer_;
  std::weak_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::weak_ptr<GameWorld> gameWorld_;
  std::weak_ptr<GameTimer> gameTimer_;
};
