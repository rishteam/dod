#include "AboutPanel.h"

namespace rl {

void AboutPanel::onImGuiRender()
{
    if(!m_show) return;

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Once, ImVec2(0.5f,0.5f));
    if(ImGui::Begin("About", nullptr, ImGuiWindowFlags_NoResize))
    {
        ImGui::Image(m_icon->getTextureID(), ImVec2(128, 128),
                     ImVec2(0, 0), ImVec2(1, -1));
        ImGui::Text("RishEngine - A 2D Game Engine");
        ImGui::Text("Members:");
        ImGui::BulletText("roy4801");
        ImGui::BulletText("ICEJJ");
        ImGui::BulletText("SunTalk");
        ImGui::BulletText("Halloworld");
        ImGui::Separator();
        ImGui::Text("https://github.com/rishteam/dod");

        // TODO: beautify the button
        if(ImGui::Button("Close"))
            hidePanel();
        ImGui::End();
    }
}

void AboutPanel::onAttach()
{
    m_icon = Texture2D::LoadTextureVFS("/icon/rish.png");
}

}
