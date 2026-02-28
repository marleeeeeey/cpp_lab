#include "ServerState.h"

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
  // convert int to std::vector<uint8_t>
  std::vector<uint8_t> payload(sizeof(numberOfClients_));
  std::memcpy(payload.data(), &numberOfClients_, sizeof(numberOfClients_));

  // add a message type to the payload
  std::vector<uint8_t> message = networkDataHandler->addTypeForBinaryMessage(GMT_NumberOfClients, payload);

  // anyway std::string_view is needed even to send binary data
  std::string_view messageStringView(reinterpret_cast<const char*>(message.data()), message.size());
  app->publish(broadcastTopicName_, messageStringView, uWS::OpCode::BINARY, false);

  SPDLOG_DEBUG("Broadcasting number of clients as uWS::OpCode::BINARY: {}", numberOfClients_);
}