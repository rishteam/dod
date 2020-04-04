#include <imgui.h>

#include "animation/editor/editor.h"
#include "log.h"

namespace rl {

void AnimationEditor::update()
{
    updateMainMenuBar();
    //
    updateOpenFileDialog();
}

void AnimationEditor::updateMainMenuBar()
{
    if(!m_showMainMentBar) return;
    //
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("File"))
    {
        if(ImGui::MenuItem("New", "Ctrl+N"))
        {

        }
        if(ImGui::MenuItem("Open", "Ctrl+O"))
        {
            m_showOpenFileDialog = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit"))
        {
        }
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Edit"))
    {

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

// TODO: Get rid of this in the future
// TODO: Figure out hot to use std::string in ImGui::TextXXX()
// TODO: fugure out how to make Input and Button work together
void AnimationEditor::updateOpenFileDialog()
{
    if(!m_showOpenFileDialog) return;
    ImGui::Begin("Open");
    constexpr int FilePathInputLen = 256;
    static char filePathInputBuf[256];
    ImGui::InputTextWithHint("File Path", "enter path here", filePathInputBuf, FilePathInputLen);
    if(ImGui::Button("Open"))
    {
        selectedOpenFile = true;
        currentOpenFilePath = filePathInputBuf;
        m_showOpenFileDialog = false;
        RL_DEBUG("{}", currentOpenFilePath);
    }
    ImGui::End();
}

}