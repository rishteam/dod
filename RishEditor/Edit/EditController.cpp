#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Math/AABB.h>
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

    if(isSelected())
    {
        auto ent = getTarget();
        auto &transform = ent.getComponent<TransformComponent>();
        // Draw Border
        Renderer2D::DrawRect(transform.translate, transform.scale);
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
    float xaxis = glm::lerp(m_cameraController->getBounds().right, m_cameraController->getBounds().left, 1.f - mr.x)
            + m_cameraController->getPosition().x;
    float yaxis = glm::lerp(m_cameraController->getBounds().bottom, m_cameraController->getBounds().top, 1.f - mr.y)
            + m_cameraController->getPosition().y;
    glm::vec2 mposInCamera{xaxis, yaxis};

    // Click to select a entity
    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        Entity frontEntity;
        float maxZ{-100.f};
        glm::vec3 upperRight, bottomLeft;
        // Iterate through all entities
        m_currentScene->m_registry.each([&](auto entityID)
        {
            Entity ent{entityID, m_currentScene.get()};
            // AABB
            m_curEntPos = ent.getComponent<TransformComponent>().translate;
            m_curSize = ent.getComponent<TransformComponent>().scale;

            if(Math::AABB2DPoint(m_curEntPos, m_curSize, mposInCamera))
            {
                // Pick the max Z one
                if (maxZ < m_curEntPos.z) {
                    frontEntity = ent;
                    maxZ = m_curEntPos.z;
                }
            }
        });

        if(frontEntity)
        {
            setTarget(frontEntity);
        }
    }

    // Entity move
    if(ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        if(isSelected())
        {
            auto ent = getTarget();
            auto &entPos = ent.getComponent<TransformComponent>().translate;
            const auto &entSize = ent.getComponent<TransformComponent>().scale;
            //
            // No moving entity
            if(!m_isNowMovingEntity)
            {
                if (Math::AABB2DPoint(entPos, entSize, mposInCamera)) {
                    m_moveEntityDiff = entPos - glm::vec3(mposInCamera, 0.f);
                    m_isNowMovingEntity = true;
                }
            }
            else
            {
                entPos = glm::vec3(mposInCamera, 0.f) + m_moveEntityDiff;
            }
        }
    }
    else
    {
        m_isNowMovingEntity = false;
        m_moveEntityDiff = glm::vec3{0.f};
    }

    // TODO: Camera pane
    if(ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        m_curMousePosInCamera = mposInCamera;

//        glm::vec2 dif = m_curMousePosInCamera - m_preMousePosInCamera;
//        RL_INFO("dif = {} {}", dif.x, dif.y);
//        m_cameraController->setPosition({1.f, 1.f});
//        m_cameraController->move(dif);

        m_preMousePosInCamera = m_curMousePosInCamera;
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
