#include "StatusBarPanel.h"

#include <Rish/ImGui.h>

namespace rl{

void StatusBarPanel::onImGuiRender() {

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin("StatusBar");
    ImGui::PopStyleVar();
    ImGui::Text("%s", m_message.c_str());
    ImGui::End();

    m_showTimeEnd = m_nowTime;
    if( m_message == "" )
        m_showTimeStart = m_nowTime;

    if(m_showTimeEnd - m_showTimeStart > 5.f ){
        m_showTimeStart = m_nowTime;
        m_message = "";
    }

}

void StatusBarPanel::sendMessage(const std::string &str) {
    m_message = str;
}

void StatusBarPanel::setNowTime(Time t){
    m_nowTime = t;
}

}

