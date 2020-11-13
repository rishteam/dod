#include "HelpPanel.h"

namespace rl {

void HelpPanel::onImGuiRender()
{

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Once, ImVec2(0.5f,0.5f));

    if(m_show)
    {
        if (!ImGui::IsPopupOpen("Help"))
            ImGui::OpenPopup("Help");

        m_show = false;
    }
    if (ImGui::BeginPopupModal("Help",
                               nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
    {

        if(ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BulletText("Left Click to select a entity, Hold Ctrl + Left Click can select multiple entities");
            ImGui::BulletText("Hold Right Mouse button can pan the Editor camera");
            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("Hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BulletText("Left Click to select a entity, Hold Ctrl + Left Click can select multiple entities");
            ImGui::BulletText("Right Click on selectd entities can trigger Action menu");
            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("Inspector", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BulletText("Edit the component");
            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("Game", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BulletText("Edit the component");
            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("Toolbar", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BulletText("Play, Pause, Stop buttons");
            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("Log", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BulletText("Logs");
            ImGui::TreePop();
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }


}

}