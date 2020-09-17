#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>
#include <Rish/Renderer/Renderer2D.h>
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
    m_cameraController->setState(m_sceneWindowFocused);
    m_cameraController->onUpdate(dt);
    m_editorGrid.onUpdate();

    glm::vec2 t = (glm::vec2)m_curEntPos;
    glm::vec2 s = (glm::vec2)m_curHalfSize;
    glm::vec2 p[4] = {
        {t.x - s.x, t.y - s.y},
        {t.x + s.x, t.y - s.y},
        {t.x + s.x, t.y + s.y},
        {t.x - s.x, t.y + s.y}
    };

    if(isSelected())
    {
        auto ent = getTarget();
        auto &transform = ent.getComponent<TransformComponent>();
        // Draw Border
        glm::vec3 p[4];
        glm::vec2 off[] = { {-1, 1}, { 1, 1}, { 1,-1}, {-1,-1} };
        for(int i = 0; i < 4; i++) // gen vertices
            p[i] = transform.translate + transform.scale * glm::vec3(off[i], 1.f) * 0.5f;
        for(int i = 0; i < 4; i++)
            Renderer2D::DrawLine(p[i], p[(i+1)%4]);
    }
}

void EditController::onImGuiRender()
{
    // states
    m_sceneWindowFocused = ImGui::IsWindowFocused();
    m_sceneWindowHovered = ImGui::IsWindowHovered();

    auto size = ImGui::GetContentRegionAvail();
    // Get mouse position
    ImVec2 curMPos = ImGui::GetMousePosRelatedToWindow(), mr{0.f, 0.f};
    mr.x = curMPos.x;
    mr.y = curMPos.y;
    // to NDC
    mr.x /= size.x;
    mr.y /= size.y;
    // To camera space
    float xaxis = glm::lerp(m_cameraController->getBounds().right, m_cameraController->getBounds().left, 1.f - mr.x);
    float yaxis = glm::lerp(m_cameraController->getBounds().bottom, m_cameraController->getBounds().top, 1.f - mr.y);
    glm::vec2 mposInCamera{xaxis, yaxis};

    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        m_currentScene->m_registry.each([&](auto entityID)
        {
            Entity ent{entityID, m_currentScene.get()};
            // AABB
            m_curEntPos = ent.getComponent<TransformComponent>().translate;
            m_curHalfSize = ent.getComponent<TransformComponent>().scale / 2.f;
            glm::vec3 upperRight = m_curEntPos + m_curHalfSize, bottomLeft = m_curEntPos - m_curHalfSize;

            if(bottomLeft.x <= mposInCamera.x && bottomLeft.y <= mposInCamera.y &&
               mposInCamera.x <= upperRight.x && mposInCamera.y <= upperRight.y)
            {
                setTarget(ent);
            }
        });
    }

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
