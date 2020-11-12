#include "StatusBarPanel.h"

#include <Rish/ImGui/ImGui.h>

namespace rl{

void StatusBarPanel::onImGuiRender() {

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin("StatusBar");
    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2{10.f, 1.f}); ImGui::SameLine();
    ImGui::Text("%s", m_message.c_str());
    ImGui::End();

    if(m_clk.getElapsedTime() > 5.f)
    {
        m_message.clear();
        m_clk.restart();
    }
}

void StatusBarPanel::sendMessage(const std::string &str)
{
    m_message = str;
    m_clk.restart();
}

}
