#include "StatusBarPanel.h"

#include <Rish/ImGui.h>

namespace rl{

void StatusBarPanel::onImGuiRender() {

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin("StatusBar");
    ImGui::PopStyleVar();

    ImGui::Text("%s", m_message.c_str());

    ImGui::End();

}

void StatusBarPanel::sendMessage(const std::string str) {
    m_message = str;
}

}

