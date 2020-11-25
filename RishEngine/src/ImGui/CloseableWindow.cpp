#include <Rish/ImGui/CloseableWindow.h>

#include <Rish/ImGui/ImGui.h>

namespace ImGui {

CloseableWindow::CloseableWindow()
{

}

CloseableWindow::~CloseableWindow()
{

}

void CloseableWindow::onImGuiRender()
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
