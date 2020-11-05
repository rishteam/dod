#include "StatusBarPanel.h"

#include <Rish/ImGui.h>

namespace rl{

void StatusBarPanel::onImGuiRender() {

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin("StatusBar");
    ImGui::PopStyleVar();

    if( !m_message.empty() ){
        ImGui::Text("%s", m_message.front().c_str());
        if( m_cnt++ > m_timelimit ){
            m_message.pop();
            m_cnt = 0;
        }
    }

    ImGui::End();

}

void StatusBarPanel::sendMessage(const std::string str) {
    m_message.push(str);
}

}

