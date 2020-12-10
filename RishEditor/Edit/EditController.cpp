#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>
//
#include <Rish/Renderer/Renderer2D.h>
//
#include <Rish/Math/AABB.h>
#include <Rish/Math/BoundingBox.h>
//
#include <Rish/ImGui.h>
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

    Renderer2D::BeginScene(m_cameraController->getCamera(), false);
    {
        // draw grid
        m_editorGrid.onUpdate(m_showGrid);
    }
    Renderer2D::EndScene();

    {
        Renderer2D::BeginScene(m_cameraController->getCamera(), true);
        SpriteRenderSystem::onRender();
        Renderer2D::EndScene();
    }

    Renderer2D::BeginScene(m_cameraController->getCamera(), false);
    {
        // Draw special entities
        drawCameraIconAndBorder(scene);
        // TODO: Draw other special entities

        // Draw attachPoint
        auto view = scene->m_registry.view<TransformComponent, RigidBody2DComponent>();
        for (auto entity : view) {
            Entity ent{entity, scene.get()};
            auto &rigid = ent.getComponent<RigidBody2DComponent>();
            auto &transform = ent.getComponent<TransformComponent>();
            //TODO: rotate Quad for collider

            float xForce = (int) rigid.force.x / 100 >= 10 ? 1 : rigid.force.x / 1000.0f;
            float yForce = (int) rigid.force.y / 100 >= 10 ? 1 : rigid.force.y / 1000.0f;

            // use Lerp to calculate the force Intensity
            float xForceIntensity = glm::lerp(0.0f, transform.scale.x/2, xForce);
            float yForceIntensity = glm::lerp(0.0f, transform.scale.y/2, yForce);

            float xForcePoint = transform.translate.x + rigid.attachPoint.x;
            float yForcePoint = transform.translate.y + rigid.attachPoint.y;

            if (rigid.showAttachPoint)
            {
                Renderer2D::DrawCircle(
                        {xForcePoint, yForcePoint}, 0.1,
                        {0.360f, 0.988f, 0.192f, 1.0f});
                auto lineColor = glm::vec4(0.f, 1.f, 1.f, 1.f);
                Renderer2D::DrawFgLine(glm::vec3(xForcePoint, yForcePoint, 0.f),
                                       glm::vec3(xForcePoint + xForceIntensity, yForcePoint, 0.f),
                                       lineColor);
                if(rigid.force.x != 0.0f)
                {
                    Renderer2D::DrawTriangle(glm::vec3(xForcePoint + xForceIntensity, yForcePoint, 0.f),
                                             glm::vec3(xForcePoint + xForceIntensity - 0.1f, yForcePoint - 0.1f, 0.f),
                                             glm::vec3(xForcePoint + xForceIntensity - 0.1f, yForcePoint + 0.1f, 0.f),
                                             lineColor);
                }
                Renderer2D::DrawFgLine(glm::vec3(xForcePoint, yForcePoint, 0.f),
                                       glm::vec3(xForcePoint, yForcePoint + yForceIntensity, 0.f),
                                       lineColor);
                if(rigid.force.y != 0.0f)
                {
                    Renderer2D::DrawTriangle(glm::vec3(xForcePoint, yForcePoint + yForceIntensity, 0.f),
                                             glm::vec3(xForcePoint + 0.1f, yForcePoint + yForceIntensity -0.1f, 0.f),
                                             glm::vec3(xForcePoint - 0.1f, yForcePoint + yForceIntensity -0.1f, 0.f),
                                             lineColor);
                }
            }
        }

        // Draw Boxcollider
        auto view2 = scene->m_registry.view<TransformComponent, Collider2DComponent>();
        for (auto entity : view2)
        {
            Entity ent{entity, scene.get()};
            auto &collider = ent.getComponent<Collider2DComponent>();
            auto &transform = ent.getComponent<TransformComponent>();
            // TODO: rotate Quad for collider
            switch(collider.type)
            {
                case Collider2DComponent::Type::Box:
                {
                        Renderer2D::DrawRotatedRect(
                                {transform.translate.x + collider.x, transform.translate.y + collider.y},
                                {collider.w, collider.h}, {1.0f, 1.0f, 0.0f, 1.0f}, transform.rotate);
                        break;
                }
                case Collider2DComponent::Type::Circle:
                {
                    Renderer2D::DrawCircleLine({transform.translate.x + collider.x, transform.translate.y + collider.y}, collider.radius * 2, {1.0f, 1.0f, 0.0f, 1.0f});
                    break;
                }
                case Collider2DComponent::Type::Polygon:
                {
                    glm::vec2 tmp[70];
                    for(int i = 0; i < collider.pointSize; i++)
                    {
                        tmp[i].x = transform.translate.x + collider.x + collider.pt[i].x;
                        tmp[i].y = transform.translate.y + collider.y + collider.pt[i].y;

                        float tmp_x = (tmp[i].x - transform.translate.x) * cos(glm::radians(transform.rotate)) - (tmp[i].y - transform.translate.y) * sin(glm::radians(transform.rotate)) + transform.translate.x;
                        float tmp_y = (tmp[i].y - transform.translate.y) * cos(glm::radians(transform.rotate)) + (tmp[i].x - transform.translate.x) * sin(glm::radians(transform.rotate)) + transform.translate.y;

                        tmp[i].x = tmp_x;
                        tmp[i].y = tmp_y;
                    }

                    Renderer2D::DrawConvex(tmp, collider.pointSize, {1.0f, 1.0f, 0.0f, 1.0f});
                    break;
                }
            }
        }


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

        // draw gizmo
        m_gizmo.setSelectedEntity(entSet);
        m_gizmo.setClickSize(glm::vec2(m_editorGrid.getOffset()/10));
        m_gizmo.onUpdate();
    }

    Renderer2D::EndScene();
}

void EditController::onImGuiRender()
{
    // states
    m_sceneWindowFocused = ImGui::IsWindowFocused();
    m_sceneWindowHovered = ImGui::IsWindowHovered();
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
    mposInWorld = glm::vec2(xaxis, yaxis);

    // Get Keyboard states
    bool isCtrlPressed = ImGui::GetIO().KeyCtrl;

    // Click to select a entity
    if(m_sceneWindowFocused &&
       m_sceneWindowHovered &&
       ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
       !m_gizmo.isMouseOnGizmo(mposInWorld))
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
            auto bound = CalculateBoundingBox2D(entPos, entSize, entRotate);
            const glm::vec3 boundPos(bound.getPosition(), 0.f);
            const glm::vec3 boundSize(bound.getScale(), 0.f);
            const glm::vec3 clickSize(m_editorGrid.getOffset()/10, m_editorGrid.getOffset()/10, 0.f);

            // Inside
            if(Math::AABB2DPoint(boundPos, boundSize, mposInWorld))
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
        }else{

            if(!isCtrlPressed)
                resetSelected();
        }

    }

    // revolve entity
    if(m_sceneWindowFocused &&
       m_sceneWindowHovered &&
       !m_gizmo.isMovingEntity()){

        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_gizmo.setMousePosBegin(mposInWorld);

        if(ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_gizmo.setMousePosEnd(mposInWorld);

        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            BoundingBox2D mouseBound = m_gizmo.getMouseBound();

            if( mouseBound.getScale().x > .001f && mouseBound.getScale().y > .001f ){

                resetTarget();

                // Iterate through all entities
                m_currentScene->m_registry.each([&](auto entityID)
                {
                    Entity ent{entityID, m_currentScene.get()};

                    auto entPos = ent.getComponent<TransformComponent>().translate;
                    auto entSize = ent.getComponent<TransformComponent>().scale;
                    auto entRotate = ent.getComponent<TransformComponent>().rotate;
                    auto entBound = CalculateBoundingBox2D(entPos, entSize, entRotate);

                    if( mouseBound.IsInside(entBound) ){
                        addTarget(ent);
                    }

                });

            }
            m_gizmo.setMousePosBegin(mposInWorld);
            m_gizmo.setMousePosEnd(mposInWorld);
        }

    }
    else{
        m_gizmo.setMousePosBegin(mposInWorld);
        m_gizmo.setMousePosEnd(mposInWorld);
    }

    // gizmo
    m_gizmo.setClickSize(glm::vec2(m_editorGrid.getOffset()/10));
    m_gizmo.onImGuiRender((m_sceneWindowFocused &&
                           m_sceneWindowHovered &&
                           ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                           isSelected()), mposInWorld);

    // Camera pane
    if(m_sceneWindowFocused &&
       ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        if(!m_isNowMovingCamera) // is not moving
        {
            m_isNowMovingCamera = true;
            m_preMPos = mposInWorld;
        }
        else
        {
            glm::vec2 vec = mposInWorld - m_preMPos;
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
            m_preMPos = mposInWorld;
        }
        else
        {
            glm::vec2 vec = mposInWorld - m_preMPos;
            m_cameraController->move(-vec);
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

} // end of namespace rl
