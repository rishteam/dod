#include <Rish/Utils/FileDialog.h>

#include "SettingPanel.h"
#include "EditorLayer.h"

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
           nullptr,
           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
    {

        ImGui::Separator();
        ImGui::AlignTextToFramePadding();
        {
            ImGui::Text("Key Setting");
            ImGui::Columns(2, "key");
            ImGui::Separator();
            ImGui::Text("Action");
            ImGui::NextColumn();
            ImGui::Text("Key");
            ImGui::NextColumn();
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
        }
        ImGui::Separator();
        ImGui::Dummy(ImVec2{1, 10});
        {
            ImGui::Text("Editor Settings");
            ImGui::Separator();
            ImGui::Columns(2, "EditorSettingsTable");

            ImGui::Text("Open Scene when editor is open");
            ImGui::Text("Scene");
            ImGui::Text("Auto save (sec.)");

            ImGui::NextColumn();

            ImGui::Checkbox("###OpenScene", &m_parent->m_editorSetting.isDefaultOpenScene);
            ImGui::InputText("##Scene", &m_parent->m_editorSetting.path, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
            if(ImGui::Button("Set"))
            {
                std::string tmpPath;
                if(FileDialog::SelectSingleFile("", nullptr, tmpPath))
                {
                    m_parent->m_editorSetting.path = FileSystem::RelativePath(tmpPath);
                }
            }
            ImGui::InputFloat("##AutoSaveSecond", &m_parent->m_editorSetting.autoSaveSecond);

            ImGui::Columns(1);
            ImGui::Separator();
        }

        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

}

}
