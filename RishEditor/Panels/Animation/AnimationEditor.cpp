#include <Rish/rlpch.h>
#include <Rish/ImGui/ImGui.h>

#include "AnimationEditor.h"

namespace rl {

AnimationEditor::AnimationEditor()
{
    m_attribWidget.addAttributeField("test", [](){
        ImGui::Text("test");
    });
}

AnimationEditor::~AnimationEditor()
{
}

void AnimationEditor::onImGuiUpdate()
{
    m_attribWidget.onImGuiRender();
}

} // end of namespace rl