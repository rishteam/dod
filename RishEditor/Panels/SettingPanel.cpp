#include "SettingPanel.h"

namespace  rl{

void SettingPanel::onImGuiRender() {

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Once, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
    if(m_show)
    {
        if (!ImGui::IsPopupOpen("Setting"))
            ImGui::OpenPopup("Setting");

        m_show = false;
    }
    if (ImGui::BeginPopupModal("Setting",
                               nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
    {

        ImGui::Separator();
        ImGui::Text("Key Setting");
        ImGui::Columns(2,"key");
        ImGui::Separator();
        ImGui::Text("Action"); ImGui::NextColumn();
        ImGui::Text("Key"); ImGui::NextColumn();
        ImGui::Separator();
        // TODO: let it can actually switch it
        ImGui::Text("New Scene");
        ImGui::Text("Open Scene");
        ImGui::Text("Save Scene");
        ImGui::Text("Save Scene as");
        ImGui::NextColumn();
        ImGui::Text("Ctrl+N");
        ImGui::Text("Ctrl+O");
        ImGui::Text("Ctrl+S");
        ImGui::Text("Ctrl+Shift+S");

        ImGui::Columns(1);
        ImGui::Separator();

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

}


}
