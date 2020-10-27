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

    auto scene = getContext();
    Renderer2D::BeginScene(m_cameraController->getCamera(), true);
    {
        auto transGroup = scene->m_registry.group<TransformComponent, RenderComponent>();
        for (auto entity: transGroup)
        {
            auto &transform = transGroup.get<TransformComponent>(entity);
            auto &render = transGroup.get<RenderComponent>(entity);

            if (render.init)
            {
                render.loadFromPath();
                render.init = false;
            }

            if (render.useTexture)
            {
                if(render.useAsSubTexture)
                {
                    if (transform.rotate != 0.f)
                        Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.m_subtexture,
                                                    render.color, transform.rotate);
                    else
                        Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_subtexture,
                                             render.color);
                }
                else // normal texture
                {
                    if (transform.rotate != 0.f)
                        Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.m_texture,
                                                    render.color, transform.rotate);
                    else
                        Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture,
                                             render.color);
                }
            }
            else
            {
                if (transform.rotate != 0.f)
                    Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.color,
                                                transform.rotate);
                else
                    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);
            }
        }
    }
    Renderer2D::EndScene();

    Renderer2D::BeginScene(m_cameraController->getCamera(), false);
    {
        m_editorGrid.onUpdate(m_showGrid);

        // Draw special entities
        drawCameraIconAndBorder(scene);
        // TODO: Draw other special entities

        // draw attachPoint
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

        // draw boxcollider
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

        // TODO: Make the max AABB
        if(isSelected())
        {
            auto &entSet = getTargets();

            for(auto &ent : entSet)
            {

                const auto &entPos = ent.getComponent<TransformComponent>().translate;
                const auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                // Get bounding box
                auto bound = CalculateBoundingBox2D(entPos, entSize, entRotate);
    //            auto boundSize = std::min(bound.getScale().x,bound.getScale().y)*0.1;
                auto clickSize = glm::vec2(m_editorGrid.getOffset()/10);
                auto clickColor = glm::vec4(1.f, 1.f, 0.f, 1.f);
                switch( m_gizmoMode ) {
                    case Gizmo::MoveMode : {
                        // Draw Border
                        Renderer2D::DrawRect(bound.getPosition(), bound.getScale(), glm::vec4(1.f));

                        auto moveQuadPos2 = glm::vec2(bound.getPosition() + clickSize*glm::vec2(.5f));
                        Renderer2D::DrawQuad(moveQuadPos2, clickSize, clickColor);
                        auto moveQuadPos3 = glm::vec3(moveQuadPos2, 2.f);
                        Renderer2D::DrawQuad(moveQuadPos3, clickSize, clickColor);

                        auto PointX = bound.getPosition() + bound.getScale() * glm::vec2(.7f, 0.f);
                        auto PointY = bound.getPosition() + bound.getScale() * glm::vec2(0.f, .7f);
                        Renderer2D::DrawFgLine(bound.getPosition(), PointX, glm::vec4(0.f, 0.f, 1.f, 1.f));
                        Renderer2D::DrawTriangle(PointX,PointX+clickSize*glm::vec2(-1.f,.5f),PointX+clickSize*glm::vec2(-1.f,-.5f), glm::vec4(0.f, 0.f, 1.f, 1.f));
                        Renderer2D::DrawFgLine(bound.getPosition(),PointY,glm::vec4(1.f, 0.f, 0.f, 1.f));
                        Renderer2D::DrawTriangle(PointY,PointY+clickSize*glm::vec2(.5f,-1.f),PointY+clickSize*glm::vec2(-.5f,-1.f), glm::vec4(1.f, 0.f, 0.f, 1.f));

                        break;
                    }
                    case Gizmo::ZoomMode :{
                        // Draw Border
                        Renderer2D::DrawRect(bound.getPosition(), bound.getScale(), glm::vec4(1.f));
                        Renderer2D::DrawCircle(bound.getPosition()+bound.getScale()*glm::vec2( 0.5f,  0.5f), clickSize.x, clickColor);
                        Renderer2D::DrawQuad(bound.getPosition()+bound.getScale()*glm::vec2( 0.5f,  0.0f), clickSize, clickColor);
                        Renderer2D::DrawCircle(bound.getPosition()+bound.getScale()*glm::vec2( 0.5f, -0.5f), clickSize.x, clickColor);
                        Renderer2D::DrawQuad(bound.getPosition()+bound.getScale()*glm::vec2( 0.0f, -0.5f), clickSize, clickColor);
                        Renderer2D::DrawCircle(bound.getPosition()+bound.getScale()*glm::vec2(-0.5f, -0.5f), clickSize.x, clickColor);
                        Renderer2D::DrawQuad(bound.getPosition()+bound.getScale()*glm::vec2(-0.5f,  0.0f), clickSize, clickColor);
                        Renderer2D::DrawCircle(bound.getPosition()+bound.getScale()*glm::vec2(-0.5f,  0.5f), clickSize.x, clickColor);
                        Renderer2D::DrawQuad(bound.getPosition()+bound.getScale()*glm::vec2( 0.0f,  0.5f), clickSize, clickColor);
                        break;
                    }
                    case Gizmo::ScaleMode:{
                        const auto tmpX = glm::vec3(entSize.x*0.5,0.f,1.f);
                        const auto tmpY = glm::vec3(0.f,entSize.y*0.5,1.f);
                        const auto sinR = std::sin(entRotate*M_PI/180);
                        const auto cosR = std::cos(entRotate*M_PI/180);
                        const auto pointX = entPos + glm::vec3(tmpX.x*cosR-tmpX.y*sinR, tmpX.x*sinR+tmpX.y*cosR, 0.f);
                        const auto pointY = entPos + glm::vec3(tmpY.x*cosR-tmpY.y*sinR, tmpY.x*sinR+tmpY.y*cosR, 0.f);

                        Renderer2D::DrawFgLine(entPos,pointX,glm::vec4(0.f,0.f,1.f,1.f));
                        Renderer2D::DrawRotatedQuad(pointX,clickSize,clickColor, entRotate);
                        Renderer2D::DrawFgLine(entPos,pointY,glm::vec4(1.f,0.f,0.f,1.f));
                        Renderer2D::DrawRotatedQuad(pointY,clickSize,clickColor, entRotate);
                        break;
                    }
                    case Gizmo::RotationMode :{
                        // Draw Circle
                        Renderer2D::DrawCircleLine(entPos, std::max(entSize.x, entSize.y));
                        Renderer2D::DrawCircle(entPos, clickSize.x, glm::vec4(0, 0.39, 0.79, 1));

                        const auto tmpPoint = glm::vec3(0.f, std::max(entSize.x, entSize.y)*0.7, 1.f);
                        const auto sinR = std::sin(glm::radians(entRotate));
                        const auto cosR = std::cos(glm::radians(entRotate));
                        Renderer2D::DrawFgLine(bound.getPosition(), glm::vec3(bound.getPosition(),0.f)+glm::vec3(tmpPoint.x*cosR-tmpPoint.y*sinR, tmpPoint.x*sinR+tmpPoint.y*cosR, 0.f), glm::vec4(1.f, 0.f, 0.f, 1.f));

                        break;
                    }
                }
            }
        }
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

    // gizmoMode

    switch (m_gizmoMode) {
        case Gizmo::MoveMode:{
            // Entity move
            if (m_sceneWindowFocused &&
                m_sceneWindowHovered &&
                ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                isSelected()) {
                // Iterate through all selected entities
                for (auto ent : m_entitySet) {
                    auto &entPos = ent.getComponent<TransformComponent>().translate;
                    const auto &transform = ent.getComponent<TransformComponent>();
                    const auto bound = CalculateBoundingBox2D(transform.translate, transform.scale, transform.rotate);
                    const glm::vec3 boundPos(bound.getPosition(), 0.f);
                    const glm::vec3 boundSize(bound.getScale(), 0.f);
                    // The entity is not moving but selected
                    if (!m_isNowMovingEntity[ent]) {
                        // Is mouse inside
                        if (Math::AABB2DPoint(boundPos, boundSize, mposInWorld)) {
                            // 當滑鼠點擊時，儲存滑鼠跟 entity 的座標
                            m_oriEntityPosition[ent] = entPos;
                            m_oriMousePosition[ent] = glm::vec3(mposInWorld, 0.f);
                            m_isNowMovingEntity[ent] = true;
                            if (Math::AABB2DPoint(entPos, boundSize * glm::vec3(.5f, .5f, .5f), mposInWorld)) {
                                m_moveEntityWeight[ent] = glm::vec3(1.f);
                            } else if (std::abs(entPos.x - mposInWorld.x) <
                                       std::abs(entPos.y - mposInWorld.y)) { // column
                                m_moveEntityWeight[ent] = glm::vec3(0.f, 1.f, 1.f);
                            } else { // row
                                m_moveEntityWeight[ent] = glm::vec3(1.f, 0.f, 1.f);
                            }
                        }
                    } else {
                        // Moving
                        entPos = m_oriEntityPosition[ent] +
                                 (glm::vec3(mposInWorld, 0.f) - m_oriMousePosition[ent]) * m_moveEntityWeight[ent];

                    };
                }

            } else {
                m_isNowMovingEntity.clear();
                m_oriEntityPosition.clear();
                m_oriMousePosition.clear();
                m_zoomEntityWeight.clear();
                m_moveEntityWeight.clear();
            }
            break;
        }
        case Gizmo::ZoomMode: {
            // Entity zoom
            if (m_sceneWindowFocused &&
                m_sceneWindowHovered &&
                ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                isSelected()) {
                // Iterate through all selected entities
                for (auto ent : m_entitySet) {
                    auto &entPos = ent.getComponent<TransformComponent>().translate;
                    auto &entSize = ent.getComponent<TransformComponent>().scale;
                    const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                    const auto &transform = ent.getComponent<TransformComponent>();
                    const auto bound = CalculateBoundingBox2D(transform.translate, transform.scale, transform.rotate);
                    glm::vec3 boundPos(bound.getPosition(), 0.f);
                    glm::vec3 boundSize(bound.getScale(), 0.f);
                    const glm::vec3 clickSize(m_editorGrid.getOffset()/10,m_editorGrid.getOffset()/10,0.f);
                    // The entity is not moving but selected
                    if (!m_isNowMovingEntity[ent]) {
                        // 當滑鼠點擊時，儲存滑鼠跟 entity 的座標
                        m_oriEntityPosition[ent] = entPos;
                        m_oriEntitySize[ent] = entSize;
                        m_oriMousePosition[ent] = glm::vec3(mposInWorld, 0.f);
                        m_oriEntityNegative[ent] = glm::vec3(entSize.x > 0 ? 1.f : -1.f, entSize.y > 0 ? 1.f : -1.f,
                                                             1.f);
                        m_isNowMovingEntity[ent] = true;
                        if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(.5f, .5f, 1.f), clickSize,
                                              mposInWorld)) { // right up
                            m_zoomEntityWeight[ent] = glm::vec3(1.f, 1.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                        } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(.5f, 0.f, 1.f), clickSize,
                                                     mposInWorld)) { // right
                            m_zoomEntityWeight[ent] = glm::vec3(1.f, 0.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(.5f, 0.f, 0.f);
                        } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(.5f, -.5f, 1.f), clickSize,
                                                     mposInWorld)) { // right down
                            m_zoomEntityWeight[ent] = glm::vec3(1.f, -1.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                        } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(0.f, -.5f, 1.f), clickSize,
                                                     mposInWorld)) { // down
                            m_zoomEntityWeight[ent] = glm::vec3(0.f, -1.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(0.f, .5f, 0.f);
                        } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(-.5f, -.5f, 1.f), clickSize,
                                                     mposInWorld)) { // left down
                            m_zoomEntityWeight[ent] = glm::vec3(-1.f, -1.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                        } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(-.5f, 0.f, 1.f), clickSize,
                                                     mposInWorld)) { // left
                            m_zoomEntityWeight[ent] = glm::vec3(-1.f, 0.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(.5f, 0.f, 0.f);
                        } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(-.5f, .5f, 1.f), clickSize,
                                                     mposInWorld)) { // left up
                            m_zoomEntityWeight[ent] = glm::vec3(-1.f, 1.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                        } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(0.f, .5f, 1.f), clickSize,
                                                     mposInWorld)) { // up
                            m_zoomEntityWeight[ent] = glm::vec3(0.f, 1.f, 0.f);
                            m_moveEntityWeight[ent] = glm::vec3(0.f, .5f, 0.f);
                        } else {
                            m_isNowMovingEntity[ent] = false;
                        }

                        m_zoomEntityWeight[ent] *= glm::vec3(entSize.x>0?1.f:-1.f,entSize.y>0?1.f:-1.f,1.f);

                    } else {
                        // zooming
                        entSize = m_oriEntitySize[ent] +
                                  (glm::vec3(mposInWorld, 0.f) - m_oriMousePosition[ent]) * m_zoomEntityWeight[ent];
                        entPos = m_oriEntityPosition[ent] +
                                 (glm::vec3(mposInWorld, 0.f) - m_oriMousePosition[ent]) * m_moveEntityWeight[ent];

                        // zoom after rotation ( zoom will broken when scale negative and degree > 180 and degree < 0
//                        boundSize = m_oriEntitySize[ent] + (glm::vec3(mposInWorld, 0.f)-m_oriMousePosition[ent])*m_zoomEntityWeight[ent];
//                        boundPos = m_oriEntityPosition[ent] + (glm::vec3(mposInWorld, 0.f)-m_oriMousePosition[ent])*m_moveEntityWeight[ent];
//
//                        auto sinR = std::sin(entRotate*M_PI/180);
//                        auto cosR = std::cos(entRotate*M_PI/180);
//                        auto newX = (boundSize.x*cosR-boundSize.y*sinR)/(cosR*cosR-sinR*sinR);
//                        auto newY = (boundSize.y*cosR-boundSize.x*sinR)/(cosR*cosR-sinR*sinR);
//                        entSize = glm::vec3(newX,newY,0)*m_oriEntityNegative[ent];
//                        entPos = boundPos;

                    };
                }
            } else {
                m_isNowMovingEntity.clear();
                m_oriEntityPosition.clear();
                m_oriEntitySize.clear();
                m_oriMousePosition.clear();
                m_zoomEntityWeight.clear();
            }
            break;
        }
        case Gizmo::ScaleMode :{
            // Entity Scale
            if (m_sceneWindowFocused &&
                m_sceneWindowHovered &&
                ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                isSelected()) {
                // Iterate through all selected entities
                for (auto ent : m_entitySet) {

                    const auto &entPos = ent.getComponent<TransformComponent>().translate;
                    auto &entSize = ent.getComponent<TransformComponent>().scale;
                    const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                    const auto tmpX = glm::vec3(entSize.x*0.5,0.f,1.f);
                    const auto tmpY = glm::vec3(0.f,entSize.y*0.5,1.f);
                    const auto sinR = std::sin(entRotate*M_PI/180);
                    const auto cosR = std::cos(entRotate*M_PI/180);
                    const auto pointX = entPos + glm::vec3(tmpX.x*cosR-tmpX.y*sinR, tmpX.x*sinR+tmpX.y*cosR, 0.f);
                    const auto pointY = entPos + glm::vec3(tmpY.x*cosR-tmpY.y*sinR, tmpY.x*sinR+tmpY.y*cosR, 0.f);

                    const glm::vec3 clickSize(m_editorGrid.getOffset()/10,m_editorGrid.getOffset()/10,0.f);

                    // The entity is not moving but selected
                    if (!m_isNowMovingEntity[ent]) {
                        // Is mouse inside
                        if (Math::AABB2DPoint(pointX, clickSize, mposInWorld)) {
                            m_oriEntityPosition[ent] = entPos;
                            m_oriEntitySize[ent] = entSize;
                            m_oriMousePosition[ent] = glm::vec3(mposInWorld, 0.f);
                            m_zoomEntityWeight[ent] = glm::vec3(1.f,0.f,1.f);
                            m_isNowMovingEntity[ent] = true;
                        }
                        if(Math::AABB2DPoint(pointY, clickSize, mposInWorld)){
                            m_oriEntityPosition[ent] = entPos;
                            m_oriEntitySize[ent] = entSize;
                            m_oriMousePosition[ent] = glm::vec3(mposInWorld, 0.f);
                            m_zoomEntityWeight[ent] = glm::vec3(0.f,1.f,1.f);
                            m_isNowMovingEntity[ent] = true;
                        }
                        m_zoomEntityWeight[ent] *= glm::vec3(entSize.x>0?1.f:-1.f,entSize.y>0?1.f:-1.f,1.f);

                    } else {
                        // Scaling 用投影量計算增加的數值
                        const auto AB = m_oriMousePosition[ent] - m_oriEntityPosition[ent];
                        const auto AC = glm::vec3(mposInWorld, 0.f) - m_oriEntityPosition[ent];
                        auto ADcoefficient = (AB.x*AC.x+AB.y*AC.y)/(AB.x*AB.x+AB.y*AB.y);
                        auto AD = glm::vec3(ADcoefficient*AB.x, ADcoefficient*AB.y, 0.f);
                        auto addLen = sqrt(AD.x*AD.x+AD.y*AD.y)*2 ;
                        addLen = addLen*((AB.x*AD.x+AB.y*AD.y)<0?-1:1) ;
                        entSize = m_oriEntitySize[ent] + glm::vec3(addLen-std::abs(m_oriEntitySize[ent].x),addLen-std::abs(m_oriEntitySize[ent].y),0.f)*m_zoomEntityWeight[ent];

//                        entSize = m_oriEntitySize[ent] + (glm::vec3(mposInWorld, 0.f) - m_oriMousePosition[ent])*m_zoomEntityWeight[ent];

                    };
                }
                break;
            } else {
                m_isNowMovingEntity.clear();
                m_oriEntityPosition.clear();
                m_oriMousePosition.clear();
                m_moveEntityWeight.clear();
            }
            break;
        }
        case Gizmo::RotationMode: {
            // Entity move
            if (m_sceneWindowFocused &&
                m_sceneWindowHovered &&
                ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
                isSelected()) {
                // Iterate through all selected entities
                for (auto ent : m_entitySet) {
                    const auto &entPosition = ent.getComponent<TransformComponent>().translate;
                    const auto &entSize = ent.getComponent<TransformComponent>().scale;
                    auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                    const auto bound = CalculateBoundingBox2D(entPosition, entSize, entRotate);
                    const glm::vec3 boundPos(bound.getPosition(), 0.f);
                    const glm::vec3 boundSize(bound.getScale(), 0.f);

                    // The entity is not moving but selected
                    if (!m_isNowMovingEntity[ent]) {

                        if (Math::AABB2DPoint(boundPos, boundSize, mposInWorld)) {
                            m_isNowMovingEntity[ent] = true;
                            m_oriEntityRotate[ent] = entRotate;
                            m_oriMouseRotate[ent] =
                                    std::atan2(mposInWorld.y - boundPos.y, mposInWorld.x - boundPos.x) * 180 / M_PI;
                        }
                    } else {
                        auto nowRotate = std::atan2(mposInWorld.y - boundPos.y, mposInWorld.x - boundPos.x) * 180 / M_PI;
                        entRotate = m_oriEntityRotate[ent] + (nowRotate - m_oriMouseRotate[ent]);
                    };
                }
            } else {
                m_isNowMovingEntity.clear();
                m_oriEntityRotate.clear();
                m_oriMouseRotate.clear();
            }
            break;
        }
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
            ImGui::Text("m_oriEntityPosition = %.2f %.2f %.2f",
                        m_oriEntityPosition[ent].x,
                        m_oriEntityPosition[ent].y,
                        m_oriEntityPosition[ent].z);
            ImGui::Text("m_oriMousePosition = %.2f %.2f %.2f",
                        m_oriMousePosition[ent].x,
                        m_oriMousePosition[ent].y,
                        m_oriMousePosition[ent].z);
            ImGui::Text("m_moveEntityWeight = %.2f %.2f %.2f",
                        m_moveEntityWeight[ent].x,
                        m_moveEntityWeight[ent].y,
                        m_moveEntityWeight[ent].z);
            ImGui::Text("mposInWorld = %.2f %.2f",
                        mposInWorld.x,
                        mposInWorld.y);
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

void EditController::changeGizmoMode(Gizmo gizmo)
{
    m_gizmoMode = gizmo;
}

void EditController::changeShowGrid()
{
    m_showGrid = !m_showGrid;
}

} // end of namespace rl
