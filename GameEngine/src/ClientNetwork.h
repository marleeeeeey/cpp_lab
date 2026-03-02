#pragma once
#include <memory>

#include "AutoReconnectionNetwork/IAutoReconnectionNetwork.h"
#include "GameRenderer/IChatRenderer.h"
#include "GameRenderer/IDebugRender.h"
#include "GameRenderer/IGameWorldRenderer.h"
#include "GameShared/ChatMessage.h"
#include "GameShared/InputPacket.h"
#include "GameUtils/GameTimer.h"
#include "NetworkDataHandler/INetworkDataHandler.h"
#include "SnowflakesSimulation.h"

class ClientNetwork {
 public:
  using OnWorldSnapshotReceivedCb = std::function<void(const WorldSnapshot&)>;

  ClientNetwork(
      const std::string& url,
      std::weak_ptr<IDebugRender> debugRender,
      std::weak_ptr<IChatRenderer> chatRenderer,
      std::weak_ptr<IGameWorldRenderer> gameWorldRenderer,
      std::weak_ptr<SnowflakesSimulation> snowflakesSimulation,
      std::weak_ptr<GameTimer> gameTimer,
      OnWorldSnapshotReceivedCb onWorldSnapshotReceivedCb);

  // ------------------------
  // Connection Interface
  // ------------------------

  void start();
  void iterate();
  void stop();

  // --------------------------
  // Transfer Data Interface
  // --------------------------

  void sendChatMessage(const ChatMessage& message);
  void sendPingFromClient();
  void sendInputPacketFromClient(const InputPacket& inputPacket);

 private:
  // ----------------------------
  // Init (Private Methods)
  // ----------------------------

  void initNetworkDataHandlers_();
  void initAutoReconnectionNetwork_();
  OnWorldSnapshotReceivedCb onWorldSnapshotReceivedCb_;

  // ---------------------
  // Ownership data
  // ---------------------

  std::unique_ptr<IAutoReconnectionNetwork> autoReconnectionNetwork_;
  std::unique_ptr<INetworkDataHandler> networkDataHandler_;
  std::string url_;

  // ---------------------
  // Shared data
  // ---------------------

  std::weak_ptr<IDebugRender> debugRender_;
  std::weak_ptr<IChatRenderer> chatRenderer_;
  std::weak_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::weak_ptr<SnowflakesSimulation> snowflakesSimulation_;
  std::weak_ptr<GameTimer> gameTimer_;
};
