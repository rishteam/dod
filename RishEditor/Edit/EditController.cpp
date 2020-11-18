#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>
//
#include <Rish/Renderer/Renderer2D.h>
//
#include <Rish/Math/AABB.h>
#include <Rish/Math/BoundingBox.h>
//
#include <Rish/ImGui/ImGui.h>
#include <Rish/Effect/Particle/ParticleSystem.h>
#include <Rish/Scene/System/SpriteRenderSystem.h>

#include "EditController.h"

namespace rl {

Ref<Texture2D> s_cameraIcon;
void EditController::drawCameraIconAndBorder(const Ref<Scene> &scene) const
{
    auto view = scene->m_registry.view<CameraComponent>();
    for(auto entity : view)
    {
        Entity ent{entity, scene.get()};
        auto &transform = ent.getComponent<TransformComponent>();

        if(m_debugShowIcon)
            Renderer2D::DrawQuad(transform.translate, {0.2f, 0.2f}, s_cameraIcon);
        Renderer2D::DrawRect(transform.translate, transform.scale, {1.f, 0.f, 0.f, 1.f});
    }
}

EditController::EditController()
{
}

void EditController::onAttach(const Ref<Scene> &scene)
{
    SceneMultiTargetPanel::onAttach(scene);
    m_cameraController = MakeRef<OrthographicCameraController>(
            Application::Get().getWindow().getAspectRatio(), false, false);
    m_editorGrid.onAttach(m_cameraController);

    // Icons
    s_cameraIcon = Texture2D::LoadTextureVFS("/icon/cameraIcon.png");
}

void EditController::onDetach()
{
    SceneMultiTargetPanel::onDetach();
}

void EditController::onUpdate(Time dt)
{
    m_cameraController->setState(m_sceneWindowFocused);
    m_cameraController->onUpdate(dt);

    auto scene = getContext();

    // Draw grid
    {
        Renderer2D::BeginScene(m_cameraController->getCamera(), false);
        m_editorGrid.onUpdate(m_showGrid);
        Renderer2D::EndScene();
    }

    // Draw sprites
    {
        Renderer2D::BeginScene(m_cameraController->getCamera(), true);
        SpriteRenderSystem::onRender();
        Renderer2D::EndScene();
    }

    // Draw special entities
    {
        Renderer2D::BeginScene(m_cameraController->getCamera(), false);

        drawCameraIconAndBorder(scene);
        // TODO: Draw other special entities

        // Draw the physics attachPoints
        auto view = scene->m_registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : view)
        {
            Entity ent{entity, scene.get()};
            auto &rigid = ent.getComponent<RigidBody2DComponent>();
            auto &transform = ent.getComponent<TransformComponent>();
            //TODO: rotate Quad for collider
            if(rigid.showAttachPoint)
                Renderer2D::Renderer2D::DrawCircle({ transform.translate.x + rigid.attachPoint.x, transform.translate.y + rigid.attachPoint.y}, 0.1,  {0.160f, 0.254f, 1.0f, 1.0f});
        }

        // Draw the box colliders
        auto view2 = scene->m_registry.view<TransformComponent, BoxCollider2DComponent>();
        for (auto entity : view2)
        {
            Entity ent{entity, scene.get()};
            auto &boxc = ent.getComponent<BoxCollider2DComponent>();
            auto &transform = ent.getComponent<TransformComponent>();
            //TODO: rotate Quad for collider
            Renderer2D::DrawRotatedRect({transform.translate.x + boxc.x, transform.translate.y + boxc.y},
                                        {boxc.w, boxc.h}, {1.0f, 1.0f, 0.0f, 1.0f}, transform.rotate);
        }

        // Delete invalid target entities
        std::set<Entity> delTarget;
        auto &entSet = getTargets();
        for(auto &ent : entSet)
        {
            // Make sure the entity is valid
            // An entity could be deleted after it is selected
            if(!ent)
            {
                delTarget.insert(ent);
                continue;
            }
        }
        for(auto &ent : delTarget)
            removeTarget(ent);

        // Draw the gizmo
        m_gizmo.setSelectedEntity(entSet);
        m_gizmo.setClickSize(glm::vec2(m_editorGrid.getOffset()/10));
        m_gizmo.onUpdate();

        Renderer2D::EndScene();
    }
}

void EditController::onImGuiRender()
{
    // Update the window states
    m_sceneWindowFocused        = ImGui::IsWindowFocused();
    m_sceneWindowHovered        = ImGui::IsWindowHovered();
    m_scenePrevLeftMouseDown    = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    m_scenePrevLeftMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    // Get mouse positions
    ImVec2 mr = ImGui::GetMousePosRelatedToWindowNormalize();
    sceneMousePosNormalize = glm::vec2{mr.x, mr.y};
    mr = ImGui::GetMousePosRelatedToWindowNormalizeCenter();
    sceneMousePosCenterNormalize = glm::vec2{mr.x, mr.y};

    // To world space
    float xaxis = glm::lerp(m_cameraController->getBounds().right, m_cameraController->getBounds().left, 1.f - sceneMousePosNormalize.x)
                  + m_cameraController->getPosition().x;
    float yaxis = glm::lerp(m_cameraController->getBounds().bottom, m_cameraController->getBounds().top, 1.f - sceneMousePosNormalize.y)
                  + m_cameraController->getPosition().y;
    mousePosInWorld = glm::vec2(xaxis, yaxis);

    // Get Keyboard states
    bool isCtrlPressed = ImGui::GetIO().KeyCtrl;

    // Click to select a entity
    if(m_sceneWindowFocused &&
       m_sceneWindowHovered &&
       ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
       !m_gizmo.isMouseOnGizmo(mousePosInWorld))
    {
        Entity bestMatch;
        glm::vec3 minSize{std::numeric_limits<float>::max(),
                          std::numeric_limits<float>::max(),
                          std::numeric_limits<float>::max()};

        // Iterate through all entities
        m_currentScene->m_registry.each([&](auto entityID)
        {
            Entity ent{entityID, m_currentScene.get()};
            // AABB
            auto entPos = ent.getComponent<TransformComponent>().translate;
            auto entSize = ent.getComponent<TransformComponent>().scale;
            auto entRotate = ent.getComponent<TransformComponent>().rotate;
            auto bound = BoundingBox2D::CalculateBoundingBox2D(entPos, entSize, entRotate);
            const glm::vec3 boundPos = bound.getPositionVec3();
            const glm::vec3 boundSize = bound.getScaleVec3();
            const glm::vec3 clickSize(m_editorGrid.getOffset()/10, m_editorGrid.getOffset()/10, 0.f);

            // Inside
            if(Math::AABB2DPoint(boundPos, boundSize, mousePosInWorld))
            {
                // Pick the smallest size one
                if(minSize.x > boundSize.x && minSize.y > boundSize.y)
                {
                    minSize = boundSize;
                    bestMatch = ent;
                }
            }
        });

        if(bestMatch)
        {
            if(!isCtrlPressed)
                resetTarget();
            addTarget(bestMatch);
        }
        else
        {
            if(!isCtrlPressed)
                resetSelected();
        }
    }

    // 區域選取
    if(m_sceneWindowFocused &&
       m_sceneWindowHovered &&
       !m_gizmo.isMovingEntity())
    {
        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_gizmo.setMousePosBegin(mousePosInWorld);

        if(ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_gizmo.setMousePosEnd(mousePosInWorld);

        // 當放開滑鼠時，區域選取 Entity
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            BoundingBox2D mouseBound = m_gizmo.getMouseBound();

            if( mouseBound.getScaleVec2().x > .001f && mouseBound.getScaleVec2().y > .001f )
            {
                resetTarget();

                // Iterate through all entities
                m_currentScene->m_registry.each([&](auto entityID)
                {
                    Entity ent{entityID, m_currentScene.get()};

                    auto entPos = ent.getComponent<TransformComponent>().translate;
                    auto entSize = ent.getComponent<TransformComponent>().scale;
                    auto entRotate = ent.getComponent<TransformComponent>().rotate;
                    auto entBound = BoundingBox2D::CalculateBoundingBox2D(entPos, entSize, entRotate);

                    if(mouseBound.isInside(entBound))
                    {
                        addTarget(ent);
                    }
                });
            }
            m_gizmo.setMousePosBegin(mousePosInWorld);
            m_gizmo.setMousePosEnd(mousePosInWorld);
        }
    }
    else
    {
        m_gizmo.setMousePosBegin(mousePosInWorld);
        m_gizmo.setMousePosEnd(mousePosInWorld);
    }

    // Update Gizmo
    m_gizmo.setClickSize(glm::vec2(m_editorGrid.getOffset()/10));
    bool isValid = m_sceneWindowFocused &&
                   m_sceneWindowHovered &&
                   ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                   isSelected();
    m_gizmo.onImGuiRender(isValid,mousePosInWorld);

    // Update Group Entity
    m_currentScene->m_registry.each([&](auto entityID)
    {
        Entity ent{entityID, m_currentScene.get()};
        if( ent.hasComponent<GroupComponent>() )
        {
            if(m_gizmo.isMoving(ent))
            {
                updateGroupEntityTransform(ent);
            }
            else
            {
                initGroupEntityTransform(ent);
            }
        }

    });

    // Camera pane
    if(m_sceneWindowFocused &&
       ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        if(!m_isNowMovingCamera) // is not moving
        {
            m_isNowMovingCamera = true;
            m_preMousePos = mousePosInWorld;
        }
        else
        {
            glm::vec2 vec = mousePosInWorld - m_preMousePos;
            m_cameraController->move(-vec);
        }
    }
    else
    {
        m_isNowMovingCamera = false;
    }

    // Preserve window padding
    auto padding = ImGui::GetStyle().WindowPadding;
    ImGui::PopStyleVar();

    if(m_debugEditorGrid)
        m_editorGrid.onImGuiRender();
    if(m_debugCameraController)
        m_cameraController->onImGuiRender();
    //
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
}

void EditController::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    if( !dispatcher.dispatch<MouseScrolledEvent>(RL_BIND_EVENT_FUNC(EditController::onMouseScrolled)) ){
        m_isNowScrollingCamera = false;
    }
    //
    if(m_sceneWindowFocused && m_sceneWindowHovered)
        m_cameraController->onEvent(e);
}

bool EditController::onMouseScrolled(MouseScrolledEvent &e)
{
    if( m_sceneWindowFocused && m_sceneWindowHovered )
    {
        if(!m_isNowScrollingCamera) // is not moving
        {
            m_isNowScrollingCamera = true;
            m_preMousePos = mousePosInWorld;
        }
        else
        {
            // Move the camera to opposite direction
            glm::vec2 dir = mousePosInWorld - m_preMousePos;
            m_cameraController->move(-dir);
        }
    }
    else
    {
        m_isNowScrollingCamera = false;
    }

    return false;
}

void EditController::changeGizmoMode(Gizmo::GizmoMode mode)
{
    m_gizmo.setGizmoMode(mode);
}

void EditController::toggleShowGrid()
{
    m_showGrid = !m_showGrid;
}

void EditController::initGroupEntityTransform(Entity groupEntity)
{
    auto &gc = groupEntity.getComponent<GroupComponent>();
    BoundingBox2D curBound;
    for(const auto& id : gc)
    {
        Entity ent = m_currentScene->getEntityByUUID(id);
        const auto &trans = ent.getComponent<TransformComponent>();
        const BoundingBox2D entBound = BoundingBox2D::CalculateBoundingBox2D(trans.translate, trans.scale, trans.rotate);
        curBound = BoundingBox2D::CombineBoundingBox2D(curBound, entBound);
    }
    auto &groupTransform = groupEntity.getComponent<TransformComponent>();
    groupTransform.translate = curBound.getPositionVec3();
    groupTransform.scale = curBound.getScaleVec3();

    for(const auto& id : gc)
    {
        Entity ent = m_currentScene->getEntityByUUID(id);
        const auto &trans = ent.getComponent<TransformComponent>();
        auto &sgc = ent.getComponent<SubGroupComponent>();
        sgc.setGroupPosition(groupTransform.translate);
        sgc.setRelativePosition(trans.translate-groupTransform.translate);
    }

}

void EditController::updateGroupEntityTransform(Entity groupEntity)
{
    auto &gc = groupEntity.getComponent<GroupComponent>();
    BoundingBox2D preBound;
    for(const auto& id : gc)
    {
        Entity ent = m_currentScene->getEntityByUUID(id);
        const auto &trans = ent.getComponent<TransformComponent>();
        const BoundingBox2D entBound = BoundingBox2D::CalculateBoundingBox2D(trans.translate, trans.scale, trans.rotate);
        preBound = BoundingBox2D::CombineBoundingBox2D(preBound, entBound);
    }
    auto &groupTransform = groupEntity.getComponent<TransformComponent>();
    auto scaleOffset = groupTransform.scale/preBound.getScaleVec3();
//    groupTransform.translate = preBound.getPositionVec3();

    for(const auto& id : gc)
    {
        Entity ent = m_currentScene->getEntityByUUID(id);
        auto &sgc = ent.getComponent<SubGroupComponent>();
        auto &trans = ent.getComponent<TransformComponent>();
        sgc.setGroupPosition(groupTransform.translate);
        trans.translate = sgc.calculateCurrentPosition();
    }

}

} // end of namespace rl
