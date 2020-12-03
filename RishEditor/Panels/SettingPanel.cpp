#include <Rish/Utils/FileDialog.h>

#include "SettingPanel.h"
#include "EditorLayer.h"

namespace  rl{

void SettingPanel::onImGuiRender()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Once, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));

    if(isPanelShowed())
    {
        if (!ImGui::IsPopupOpen(m_panelTitle))
        {
            readData();
            ImGui::OpenPopup(m_panelTitle);
        }
        hidePanel();
    }

    if (ImGui::BeginPopupModal(m_panelTitle,
           nullptr,
           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        if(ImGui::BeginTabBar("##SetingTab", ImGuiTabBarFlags_None))
        {
            if(ImGui::BeginTabItem("Key"))
            {
                ImGui::BeginChild("##KeyChild", ImVec2(0,io.DisplaySize.y * 0.415f), true);
                {
                    ImGui::Columns(2, "keySettingTable");
                    ImGui::Text("Action");
                    ImGui::NextColumn();
                    ImGui::Text("Key");
                    ImGui::NextColumn();
                    ImGui::Separator();

                    // TODO: let it can actually switch it
//                    ImGui::Text("New Scene");
//                    ImGui::NextColumn();
//                    ImGui::Text("Ctrl+N");
//                    ImGui::NextColumn();
//
//                    ImGui::Text("Open Scene");
//                    ImGui::NextColumn();
//                    ImGui::Text("Ctrl+O");
//                    ImGui::NextColumn();

                    ImGui::Text("Save Scene");
                    ImGui::NextColumn();
                    ImGui::Text("Ctrl+S");
                    ImGui::NextColumn();

                    ImGui::Text("Save Scene as");
                    ImGui::NextColumn();
                    ImGui::Text("Ctrl+Shift+S");
                    ImGui::NextColumn();

                    ImGui::Text("Select All");
                    ImGui::NextColumn();
                    ImGui::Text("Ctrl+A");
                    ImGui::NextColumn();

                    ImGui::Text("Creat Entity");
                    ImGui::NextColumn();
                    ImGui::Text("Ctrl+N");
                    ImGui::NextColumn();

                    ImGui::Text("Paste");
                    ImGui::NextColumn();
                    ImGui::Text("Ctrl+V");
                    ImGui::NextColumn();

                    ImGui::Text("Delete");
                    ImGui::NextColumn();
                    ImGui::Text("Delete");
                    ImGui::NextColumn();

                    ImGui::Text("Group Entity");
                    ImGui::NextColumn();
                    ImGui::Text("Ctrl+G");
                    ImGui::NextColumn();

                    ImGui::Columns(1);
                    ImGui::Separator();
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            if(ImGui::BeginTabItem("Editor"))
            {
                ImGui::BeginChild("##EditorChild", ImVec2(0,io.DisplaySize.y * 0.415f), true);
                {
                    ImGui::Text("Editor Settings");
                    ImGui::Separator();
                    ImGui::Columns(2, "EditorSettingsTable");

                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Open Scene when editor is open");
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Scene");
                    ImGui::AlignTextToFramePadding();
                    ImGui::AlignTextToFramePadding();
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
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::PushItemWidth(300);
        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
            writeData();
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopItemWidth();

        ImGui::EndPopup();
    }

}

void SettingPanel::readData()
{
    m_shortCut = &m_parent->m_sceneAction;
}

void SettingPanel::writeData()
{
    // TODO: implement write data function
}

}
