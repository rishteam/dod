#include "HelpPanel.h"

namespace rl {

void HelpPanel::onImGuiRender()
{
    if(!m_show) return;

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Once, ImVec2(0.5f,0.5f));
    if(ImGui::Begin("Help", nullptr, ImGuiWindowFlags_NoResize))
    {
        ImGui::Text("TODO");

        // TODO: beautify the button
        if(ImGui::Button("Close"))
            hidePanel();
        ImGui::End();
    }
}

}