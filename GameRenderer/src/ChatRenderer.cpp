#include "ChatRenderer.h"

#include <imgui.h>

#include "TimeUtils.h"

void ChatRenderer::addMessage(const ChatMessage& message) {
  chatHistory_.push_back(message);
}

void ChatRenderer::render() {
  // ------------------------
  // Configure ImGui window
  // ------------------------

  ImGuiIO& io = ImGui::GetIO();

  // This window always occupies the entire screen of the host window
  ImVec2 halfDisplaySize = io.DisplaySize;
  halfDisplaySize.y /= 2;
  ImGui::SetNextWindowPos(ImVec2{0, halfDisplaySize.y}, ImGuiCond_Always);
  ImGui::SetNextWindowSize(halfDisplaySize, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.0f);  // Background fully transparent

  // Disable window interaction
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoTitleBar;

  // --------------
  // Start window
  // --------------

  ImGui::Begin("Chat Window", nullptr, flags);

  // ------------------
  // Message history
  // ------------------

  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                    ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
  for (const auto& chatMessage : chatHistory_) {
    // Calculate delivery time
    auto deliveryTime = chatMessage.receivedTimestamp - chatMessage.sentTimestamp;
    auto deliveryTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(deliveryTime).count();

    const bool useExtendedFormat = false;
    if (useExtendedFormat) {
      // [Time] [Msg RTT] [Total Messages From This User] Name: Message
      ImGui::TextWrapped("[%s] [+%lldms] [%d] %s: %s",
                         TimeUtils::timeToStringHHMMSSMS(chatMessage.sentTimestamp).c_str(),
                         deliveryTimeMs,
                         chatMessage.sender.messagesSent,
                         chatMessage.sender.name.c_str(),
                         chatMessage.message.c_str());
    } else {
      ImGui::TextWrapped("%s: %s", chatMessage.sender.name.c_str(), chatMessage.message.c_str());
    }
  }
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);
  ImGui::EndChild();

  // -------------
  // Input field
  // -------------

  static char inputBuf[256] = "";
  auto handleSend = [&]() {
    if (inputBuf[0] != '\0') {
      if (onMessageSentCallback) {
        onMessageSentCallback(inputBuf);
      }

      std::fill(std::begin(inputBuf), std::end(inputBuf), '\0');
      ImGui::SetKeyboardFocusHere(-1);
    }
  };

  const float sendW = 90.0f;                                                                // Width of the "Send" button
  const float spacing = ImGui::GetStyle().ItemSpacing.x;                                    // Default spacing between elements
  const float inputW = std::max(1.0f, ImGui::GetContentRegionAvail().x - sendW - spacing);  // Width of the input field
  ImGui::SetNextItemWidth(inputW);

  if (ImGui::InputText("##Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
    handleSend();
  }

  // --------------------------
  // Send button on same line
  // --------------------------

  ImGui::SameLine();
  if (ImGui::Button("Send", ImVec2(sendW, ImGui::GetFrameHeight()))) {
    handleSend();
  }

  ImGui::End();
}