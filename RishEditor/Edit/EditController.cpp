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
    m_editorGrid.onUpdate();

    auto scene = getContext();
    auto transGroup = scene->m_registry.group<TransformComponent, RenderComponent>();
    for(auto entity: transGroup)
    {
        auto &transform = transGroup.get<TransformComponent>(entity);
        auto &render = transGroup.get<RenderComponent>(entity);

        // TODO: make these into entt function
        if(render.init)
        {
            render.m_texture = Texture2D::LoadTextureVFS(render.texturePath);
            render.m_shader = Shader::LoadShaderVFS(render.vertPath, render.fragPath);
            render.init = false;
        }

        if(render.m_texture)
        {
            if(transform.rotate != 0.f)
                Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color, transform.rotate);
            else
                Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color);
        }
        else
        {
            if(transform.rotate != 0.f)
                Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.color, transform.rotate);
            else
                Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);
        }
    }

    // Draw special entities
    drawCameraIconAndBorder(scene);

    // draw boxcollider
    auto view = scene->m_registry.view<TransformComponent, BoxCollider2DComponent>();
    for(auto entity : view)
    {
        Entity ent{entity, scene.get()};
        auto &boxc = ent.getComponent<BoxCollider2DComponent>();
        auto &transform = ent.getComponent<TransformComponent>();
        //TODO: rotate Quad for collider
        Renderer2D::DrawRotatedRect({transform.translate.x + boxc.x, transform.translate.y + boxc.y}, {boxc.w, boxc.h},  {1.0f, 1.0f, 0.0f, 1.0f}, transform.rotate);
    }

    // TODO: Make the max AABB
    if(isSelected())
    {
        auto &entSet = getTargets();
        for(auto &ent : entSet)
        {
            auto &transform = ent.getComponent<TransformComponent>();
            // Get bounding box
            auto bound = CalculateBoundingBox2D(transform.translate, transform.scale, transform.rotate);
            // Draw Border
            Renderer2D::DrawRect(bound.getPosition(), bound.getScale(), glm::vec4(1.f));
        }
    }
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
    glm::vec2 mposInWorld{xaxis, yaxis};

    // Get Keyboard states
    bool isCtrlPressed = ImGui::GetIO().KeyCtrl;

    // Click to select a entity
    if(m_sceneWindowFocused &&
       m_sceneWindowHovered &&
       ImGui::IsMouseClicked(ImGuiMouseButton_Left))
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
            m_curEntPos = ent.getComponent<TransformComponent>().translate;
            m_curSize = ent.getComponent<TransformComponent>().scale;
            // Inside
            if(Math::AABB2DPoint(m_curEntPos, m_curSize, mposInWorld))
            {
                if(m_entitySet.count(ent))
                {
                    removeTarget(ent);
                }
                // Pick the smallest size one
                if(minSize.x > m_curSize.x && minSize.y > m_curSize.y)
                {
                    minSize = m_curSize;
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
    }

    // TODO: Support multiple entities move
    // Entity move
    if(m_sceneWindowFocused &&
       m_sceneWindowHovered &&
       ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
       isSelected())
    {
        // Iterate through all selected entities
        for(auto ent : m_entitySet)
        {
            auto &entPos = ent.getComponent<TransformComponent>().translate;
            const auto &entSize = ent.getComponent<TransformComponent>().scale;
            // The entity is not moving but selected
            if (!m_isNowMovingEntity[ent])
            {
                // Is mouse inside
                if (Math::AABB2DPoint(entPos, entSize, mposInWorld))
                {
                    // 當滑鼠點擊時，儲存滑鼠座標跟 entity 的差距
                    m_moveEntityDiff[ent] = entPos - glm::vec3(mposInWorld, 0.f);
                    m_isNowMovingEntity[ent] = true;
                }
            }
            else
            {
                // Moving
                entPos = glm::vec3(mposInWorld, 0.f) + m_moveEntityDiff[ent];
            }
        }
    }
    else
    {
        m_isNowMovingEntity.clear();
        m_moveEntityDiff.clear();
    }

    // Camera pane
    if(m_sceneWindowFocused &&
       ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        if(!m_isNowMovingCamera) // is not moving
        {
            m_isNowMovingCamera = true;
            m_preMPos = mposInWorld;
        }
        else if(m_isNowMovingCamera &&    // is moving
                mposInWorld != m_preMPos) // and mpos has been changed
        {
            // Natural pane (inverse)
            glm::vec2 vec = mposInWorld - m_preMPos;
            m_cameraController->move(-vec);
            m_isNowMovingCamera = false;
        }
    }
    else
    {
        m_isNowMovingCamera = false;
    }

    // Preserve window padding
    auto padding = ImGui::GetStyle().WindowPadding;
    ImGui::PopStyleVar();
    //
    if(m_debugEditorController)
    {
        ImGui::Begin("EditController");
        ImGui::Text("Entity Move");
        ImGui::Indent();
        for(auto ent : m_entitySet)
        {
            ImGui::PushID((void*)&ent);
            ImGui::Text("%s", ent.getUUID().to_c_str());
            ImGui::Indent();
            ImGui::Text("m_isNowMovingEntity = %d", m_isNowMovingEntity[ent]);
            ImGui::Text("m_moveEntityDiff = %.2f %.2f %.2f",
                        m_moveEntityDiff[ent].x,
                        m_moveEntityDiff[ent].y,
                        m_moveEntityDiff[ent].z);
            ImGui::Unindent();
            ImGui::PopID();
        }
        ImGui::Unindent();
        ImGui::End();
    }
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
    dispatcher.dispatch<MouseScrolledEvent>(RL_BIND_EVENT_FUNC(EditController::onMouseScrolled));
    //
    if(m_sceneWindowFocused && m_sceneWindowHovered)
        m_cameraController->onEvent(e);
}

bool EditController::onMouseScrolled(MouseScrolledEvent &e)
{
    // TODO: camera translate when zooming
//    RL_INFO("onMouseScrolled {:.2f}, {:.2f}", sceneMousePosCenterNormalize.x, sceneMousePosCenterNormalize.y);
//    if(e.yOffset >= 0.f)
//        m_cameraController->move(-sceneMousePosCenterNormalize);
//    else
//        m_cameraController->move(sceneMousePosCenterNormalize);
    return false;
}

} // end of namespace rl
