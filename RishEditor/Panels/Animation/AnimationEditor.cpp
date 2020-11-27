#include "AnimationEditor.h"

#include <Rish/Animation/Component.h>
#include <Rish/ImGui/ImGui.h>
#include <Rish/Utils/FileDialog.h>
#include <Rish/Utils/Serialize.h>
#include <Rish/Utils/String.h>

namespace rl {

AnimationEditor::AnimationEditor() : ImGuiWindow("Animation Editor")
{
    m_attribWidget.addAttributeField("Frame Count", [=](){
        ImGui::Text("%d", m_editTarget ? m_editTarget->getFrameCount() : -1);
    });
    //
    m_attribWidget.addAttributeField("Duration", [=](){
        if(!m_editTarget) return;
        ImGui::InputFloat("##AnimationDuration", &m_editTarget->duration);
    });
    //
    m_attribWidget.addAttributeField("Reverse Duration", [=](){
        if(!m_editTarget) return;
        ImGui::InputFloat("##AnimationReverseDuration", &m_editTarget->reverseDuration);
    });
    //
    m_attribWidget.addAttributeField("Is Loop", [=](){
        if(!m_editTarget) return;
        ImGui::Checkbox("##AnimationLoop", &m_editTarget->loop);
    });
    //
    m_attribWidget.addAttributeField("Is reverse", [=](){
        if(!m_editTarget) return;
        ImGui::Checkbox("##AnimationReverse", &m_editTarget->reverse);
    });
}

AnimationEditor::~AnimationEditor()
{
}

void AnimationEditor::onImGuiUpdate()
{
    std::string path;
    //
    if(ImGui::Button("Load"))
    {
        if(FileDialog::SelectSingleFile("rani",
                                      (FileSystem::GetCurrentDirectory() + "\\assets\\animation").c_str(),
                                      path))
        {
            loadFromFile("Animation2D", *m_editTarget, path);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Save as"))
    {
        if(FileDialog::SelectSaveFile("rani",
          (FileSystem::GetCurrentDirectory() + "\\assets\\animation").c_str(),
          path))
        {
            if(!String::endswith(path, ".rani"))
                path += ".rani";
            saveToFile("Animation2D", *m_editTarget, path);
        }
    }


    m_attribWidget.onImGuiRender();
}

void AnimationEditor::setTargetComponent(Animation2DComponent *target)
{
    m_editTarget = target;
}

void AnimationEditor::resetTargetComponent()
{
    m_editTarget = nullptr;
}

void AnimationEditor::loadAnimation()
{

}

} // end of namespace rl