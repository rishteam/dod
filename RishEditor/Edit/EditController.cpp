#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>
#include <Rish/ImGui.h>

#include "EditController.h"

namespace rl {

EditController::EditController()
{
}

void EditController::onAttach(const Ref<Scene> &scene)
{
    SceneTargetPanel::onAttach(scene);
    m_cameraController = MakeRef<OrthographicCameraController>(
        Application::Get().getWindow().getAspectRatio());
    m_editorGrid.onAttach(m_cameraController);
}

void EditController::onDetach()
{
    SceneTargetPanel::onDetach();
}

void EditController::onUpdate(Time dt)
{
    m_editorGrid.onUpdate();
    m_cameraController->setState(m_sceneWindowFocused);
    m_cameraController->onUpdate(dt);
}

void EditController::onImGuiRender()
{
    // states
    m_sceneWindowFocused = ImGui::IsWindowFocused();
    m_sceneWindowHovered = ImGui::IsWindowHovered();

    auto size = ImGui::GetContentRegionAvail();
    // Get mouse position
    ImVec2 curMPos = ImGui::GetMousePosRelatedToWindow(), mr;
    // Convert the origin to center
    float hW = size.x / 2.f, hH = size.y / 2.f;
    mr.x = curMPos.x - hW;
    mr.y = hH - curMPos.y;
    // to NDC
    mr.x /= hW;
    mr.y /= hH;

    // Preserve window padding
    auto padding = ImGui::GetStyle().WindowPadding;
    ImGui::PopStyleVar();
    //
    if(m_debugEditorGrid)
        m_editorGrid.onImGuiRender();
    if(m_debugCameraController)
        m_cameraController->onImGuiRender();
    //
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
}

void EditController::onEvent(Event &e)
{
    if(m_sceneWindowFocused && m_sceneWindowHovered)
        m_cameraController->onEvent(e);
}

} // end of namespace rl
