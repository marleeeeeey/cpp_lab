#include "DoubleQueueNetwork.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

#include "NetworkTransport/ITransport.h"

DoubleQueueNetwork::DoubleQueueNetwork() {
  networkTransport_ = ITransport::create();
  SPDLOG_TRACE("Network transport created");

  // -------------------------------------
  // Setup WebSocket transport callbacks
  // -------------------------------------

  networkTransport_->onOpen = [this]() {
    SPDLOG_TRACE("onOpen");
    connected_.store(true);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Connected});
  };
  networkTransport_->onError = [this](std::string_view errorMsg) {
    connected_.store(false);
    SPDLOG_ERROR("onError: {}", errorMsg);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Error, std::string(errorMsg)});
  };
  networkTransport_->onText = [this](std::string_view msg) {
    SPDLOG_TRACE("onText: {}", msg);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::TextMessage, std::string(msg)});
  };
  networkTransport_->onBinary = [this](uint8_t* data, int size) {
    SPDLOG_TRACE("onBinary: size={}", size);
    inboundEventQueue_.enqueue(NetEvent{
        .type = NetEvent::Type::BinaryMessage,
        .textPayload = std::string(),
        .binaryPayload = std::vector<uint8_t>(data, data + size)});
  };
  networkTransport_->onClose = [this](int code, std::string_view reason) {
    connected_.store(false);
    SPDLOG_TRACE("onClose. Code={}, Reason={}", code, reason);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Disconnected, std::string(reason)});
  };
}

void DoubleQueueNetwork::start(std::string_view url) {
  SPDLOG_TRACE("DoubleQueueNetwork::start");
  if (running_.exchange(true) && connected_.load() == true) {
    return;  // already started
  }

  connected_.store(false);

  // Start a thread to accept incoming messages
  networkTransport_->connect(url);
}

void DoubleQueueNetwork::stop() {
  SPDLOG_TRACE("DoubleQueueNetwork::stop");
  if (!running_.exchange(false)) {
    return;
  }

  if (networkTransport_) {
    networkTransport_->close();  // blocking
  }
  networkTransport_.reset();
}

bool DoubleQueueNetwork::poll(NetEvent& out) {
  return inboundEventQueue_.try_dequeue(out);
}

void DoubleQueueNetwork::send(std::string_view msg) {
  SPDLOG_TRACE("DoubleQueueNetwork::sendText. msg={}", msg);
  if (!networkTransport_) {
    SPDLOG_ERROR("Network transport isn't initialized");
  }

  outboundEventQueue_.enqueue(NetEvent{NetEvent::Type::TextMessage, std::string(msg)});
}

void DoubleQueueNetwork::send(std::vector<uint8_t> payload) {
  SPDLOG_TRACE("DoubleQueueNetwork::sendBinary. msg.size={}", payload.size());
  if (!networkTransport_) {
    SPDLOG_ERROR("Network transport isn't initialized");
  }

  outboundEventQueue_.enqueue(NetEvent{NetEvent::Type::BinaryMessage, std::string(), payload});
}

void DoubleQueueNetwork::drainOutboundQueue() {
  if (!networkTransport_) {
    SPDLOG_ERROR("Network transport isn't initialized");
    return;
  }

  // Set queue processing limit
  constexpr int kMaxMessagesPerDrain = 512;

  int processed = 0;
  while (processed < kMaxMessagesPerDrain) {
    NetEvent ev;
    if (!outboundEventQueue_.try_dequeue(ev)) {
      break;
    }

    SPDLOG_TRACE("Draining outbound queue. Event type={}", magic_enum::enum_name(ev.type));

    if (ev.type != NetEvent::Type::TextMessage && ev.type != NetEvent::Type::BinaryMessage) {
      SPDLOG_WARN("Unexpected event type: {}. Drop this message", magic_enum::enum_name(ev.type));
      outboundEventQueue_.enqueue(std::move(ev));
      break;
    }

    // Try to send a text message
    if (ev.type == NetEvent::Type::TextMessage) {
      auto res = networkTransport_->sendText(ev.textPayload);
      if (res != ITransport::SendResult::Success) {
        SPDLOG_WARN("Failed to send message. Type: {}, Payload: {}. Message back to the queue",
                    magic_enum::enum_name(ev.type), ev.textPayload);
        outboundEventQueue_.enqueue(std::move(ev));
        break;
      }
    }

    // Try to send a binary message
    if (ev.type == NetEvent::Type::BinaryMessage) {
      auto res = networkTransport_->sendBinary(ev.binaryPayload);
      if (res != ITransport::SendResult::Success) {
        SPDLOG_WARN("Failed to send message. Type: {}, Payload size: {}. Message back to the queue",
                    magic_enum::enum_name(ev.type), ev.binaryPayload.size());
        outboundEventQueue_.enqueue(std::move(ev));
      }
    }

    ++processed;
  }
}
