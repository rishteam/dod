#include <Rish/ImGui/ImGuiWindow.h>

#include <Rish/ImGui/ImGui.h>

namespace rl {

ImGuiWindow::ImGuiWindow()
{

}

ImGuiWindow::~ImGuiWindow()
{

}

void ImGuiWindow::onImGuiRender()
{
    if(!m_showEditor)
        return;

    if(ImGui::Begin("Animation RishEditor", &m_showEditor, ImGuiWindowFlags_NoCollapse))
    {
        onImGuiUpdate();
        ImGui::End();
        return; // early out
    }

    ImGui::End();
}

} // end of namespace ImGui
