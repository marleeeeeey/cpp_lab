#include "ServerState.h"

#include "GameSerialization/MemcpySerialization.h"
#include "GameShared/GameMessageTypes.h"

ServerState::ServerState() {
  app = std::make_shared<uWS::App>();
  networkDataHandler = INetworkDataHandler::create();
}

void ServerState::incrementNumberOfClients() {
  numberOfClients_++;
  if (numberOfClients_ > maxNumberOfClients_) {
    maxNumberOfClients_ = numberOfClients_;
  }
  SPDLOG_INFO("Number of clients (++): {}. Max number of clients: {}", numberOfClients_, maxNumberOfClients_);
  broadcastNumberClients_();
}

void ServerState::decrementNumberOfClients() {
  numberOfClients_--;
  SPDLOG_INFO("Number of clients (--): {}. Max number of clients: {}", numberOfClients_, maxNumberOfClients_);
  broadcastNumberClients_();
}

const std::string_view& ServerState::getBroadcastTopicName() const { return broadcastTopicName_; }

void ServerState::broadcastNumberClients_() {
  // serialize the number of clients
  auto payload = GameSerialization::serializeMemcpy(numberOfClients_);

  // add a message type to the payload
  auto typedPayload = networkDataHandler->addTypeForBinaryMessage(GMT_NumberOfClients, payload);

  // anyway std::string_view is needed even to send binary data
  std::string_view messageStringView(reinterpret_cast<const char*>(typedPayload.data()), typedPayload.size());
  app->publish(broadcastTopicName_, messageStringView, uWS::OpCode::BINARY, false);

  SPDLOG_DEBUG("Broadcasting number of clients as uWS::OpCode::BINARY: {}", numberOfClients_);
}