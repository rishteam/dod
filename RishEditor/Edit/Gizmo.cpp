//
#include <Rish/Renderer/Renderer2D.h>
//
#include <Rish/Math/AABB.h>
#include <Rish/Math/BoundingBox.h>
//
#include "Gizmo.h"

namespace rl{

Gizmo::Gizmo()
{
}

void Gizmo::onImGuiRender(bool isValid, glm::vec2 mousePosInWorld)
{
    if(!isValid)
    {
        m_isNowMovingEntity.clear();
        m_oriEntityPosition.clear();
        m_oriEntitySize.clear();
        m_oriEntityNegative.clear();
        m_oriEntityRotate.clear();
        m_oriMousePosition.clear();
        m_oriMouseRotate.clear();
        m_moveEntityWeight.clear();
        m_zoomEntityWeight.clear();
        return;
    }

    BoundingBox2D mouseBound = BoundingBox2D::CalculateBoundingFromPoint(m_mousePosBegin, m_mousePosEnd);
    if( mouseBound.getScale().x > .001f || mouseBound.getScale().y > .001f )
    {
        return;
    }

    // Gizmo Mode
    switch (m_gizmoMode)
    {
        case GizmoMode::MoveMode:
        {
            // Entity move
            auto [boundPos, boundSize] = calculateNewBoundVec3();

            for (auto ent : m_entitySet)
            {
                auto &entPos = ent.getComponent<TransformComponent>().translate;

                // The entity is not moving but selected
                if (!m_isNowMovingEntity[ent])
                {
                    m_oriEntityPosition[ent] = entPos;
                    m_oriMousePosition[ent] = glm::vec3(mousePosInWorld, 0.f);
                    m_isNowMovingEntity[ent] = true;
                    //
                    if (Math::AABB2DPoint(boundPos+m_clickSize3*glm::vec3(.5f), m_clickSize3, mousePosInWorld))
                    {
                        m_moveEntityWeight[ent] = glm::vec3(1.f);
                    }
                    // row
                    else if(Math::AABB2DPoint(boundPos+boundSize*glm::vec3(.5f,0.f,0.f)+m_clickSize3*glm::vec3(1.5f,0.f,0.f),
                                              boundSize*glm::vec3(.5f,0.f,0.f) + m_clickSize3*glm::vec3(3.f,1.f,0.f), mousePosInWorld))
                    {
                        m_moveEntityWeight[ent] = glm::vec3(1.f, 0.f, 1.f);
                    }
                    // column
                    else if (Math::AABB2DPoint(boundPos+boundSize*glm::vec3(0.f,.5f,0.f)+m_clickSize3*glm::vec3(0.f,1.5f,0.f),
                                               boundSize*glm::vec3(0.f,.5f,0.f) + m_clickSize3*glm::vec3(1.f,3.f,0.f), mousePosInWorld))
                    {
                        m_moveEntityWeight[ent] = glm::vec3(0.f, 1.f, 1.f);
                    }
                    else
                    {
                        m_isNowMovingEntity.erase(ent);
                    }

                } else {
                    // Moving
                    entPos = m_oriEntityPosition[ent] +
                             (glm::vec3(mousePosInWorld, 0.f) - m_oriMousePosition[ent]) * m_moveEntityWeight[ent];

                };
            }

            break;
        }
        case GizmoMode::ZoomMode:
        {
            for (auto ent : m_entitySet)
            {
                auto &entPos = ent.getComponent<TransformComponent>().translate;
                auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                const auto &transform = ent.getComponent<TransformComponent>();
                const auto bound = BoundingBox2D::CalculateBoundingBox2D(transform.translate, transform.scale, transform.rotate);
                glm::vec3 boundPos(bound.getPosition(), 0.f);
                glm::vec3 boundSize(bound.getScale(), 0.f);

                // The entity is not moving but selected
                if (!m_isNowMovingEntity[ent]) {
                    // 當滑鼠點擊時，儲存滑鼠跟 entity 的座標
                    m_oriEntityPosition[ent] = entPos;
                    m_oriEntitySize[ent] = entSize;
                    m_oriMousePosition[ent] = glm::vec3(mousePosInWorld, 0.f);
                    m_oriEntityNegative[ent] = glm::vec3(entSize.x > 0 ? 1.f : -1.f, entSize.y > 0 ? 1.f : -1.f,
                                                         1.f);
                    m_isNowMovingEntity[ent] = true;
                    if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(.5f, .5f, 1.f), m_clickSize3,
                                          mousePosInWorld)) { // right up
                        m_zoomEntityWeight[ent] = glm::vec3(1.f, 1.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                    } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(.5f, 0.f, 1.f), m_clickSize3,
                                                 mousePosInWorld)) { // right
                        m_zoomEntityWeight[ent] = glm::vec3(1.f, 0.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(.5f, 0.f, 0.f);
                    } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(.5f, -.5f, 1.f), m_clickSize3,
                                                 mousePosInWorld)) { // right down
                        m_zoomEntityWeight[ent] = glm::vec3(1.f, -1.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                    } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(0.f, -.5f, 1.f), m_clickSize3,
                                                 mousePosInWorld)) { // down
                        m_zoomEntityWeight[ent] = glm::vec3(0.f, -1.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(0.f, .5f, 0.f);
                    } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(-.5f, -.5f, 1.f), m_clickSize3,
                                                 mousePosInWorld)) { // left down
                        m_zoomEntityWeight[ent] = glm::vec3(-1.f, -1.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                    } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(-.5f, 0.f, 1.f), m_clickSize3,
                                                 mousePosInWorld)) { // left
                        m_zoomEntityWeight[ent] = glm::vec3(-1.f, 0.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(.5f, 0.f, 0.f);
                    } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(-.5f, .5f, 1.f), m_clickSize3,
                                                 mousePosInWorld)) { // left up
                        m_zoomEntityWeight[ent] = glm::vec3(-1.f, 1.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(.5f, .5f, 0.f);
                    } else if (Math::AABB2DPoint(boundPos + boundSize * glm::vec3(0.f, .5f, 1.f), m_clickSize3,
                                                 mousePosInWorld)) { // up
                        m_zoomEntityWeight[ent] = glm::vec3(0.f, 1.f, 0.f);
                        m_moveEntityWeight[ent] = glm::vec3(0.f, .5f, 0.f);
                    } else {
                        m_isNowMovingEntity.erase(ent);
                    }

                    m_zoomEntityWeight[ent] *= glm::vec3(entSize.x>0?1.f:-1.f,entSize.y>0?1.f:-1.f,1.f);

                } else {
                    // zooming
                    entSize = m_oriEntitySize[ent] +
                              (glm::vec3(mousePosInWorld, 0.f) - m_oriMousePosition[ent]) * m_zoomEntityWeight[ent];
                    entPos = m_oriEntityPosition[ent] +
                             (glm::vec3(mousePosInWorld, 0.f) - m_oriMousePosition[ent]) * m_moveEntityWeight[ent];

                }


            }
            break;
        }
        case GizmoMode::ScaleMode:
        {
            for (auto ent : m_entitySet)
            {
                const auto &entPos = ent.getComponent<TransformComponent>().translate;
                auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                const auto tmpX = glm::vec3(entSize.x*0.5,0.f,1.f) + m_clickSize3*glm::vec3(3.f, 0.f, 1.f);
                const auto tmpY = glm::vec3(0.f,entSize.y*0.5,1.f) + m_clickSize3*glm::vec3(0.f, 3.f, 1.f);
                const auto sinR = std::sin(entRotate*M_PI/180);
                const auto cosR = std::cos(entRotate*M_PI/180);
                const auto pointX = entPos + glm::vec3(tmpX.x*cosR-tmpX.y*sinR, tmpX.x*sinR+tmpX.y*cosR, 0.f);
                const auto pointY = entPos + glm::vec3(tmpY.x*cosR-tmpY.y*sinR, tmpY.x*sinR+tmpY.y*cosR, 0.f);

                // The entity is not moving but selected
                if (!m_isNowMovingEntity[ent]) {
                    // Is mouse inside
                    if (Math::AABB2DPoint(pointX, m_clickSize3, mousePosInWorld)) {
                        m_oriEntityPosition[ent] = entPos;
                        m_oriEntitySize[ent] = entSize;
                        m_oriMousePosition[ent] = pointX;
                        m_zoomEntityWeight[ent] = glm::vec3(1.f,0.f,1.f);
                        m_isNowMovingEntity[ent] = true;
                    }
                    else if(Math::AABB2DPoint(pointY, m_clickSize3, mousePosInWorld)){
                        m_oriEntityPosition[ent] = entPos;
                        m_oriEntitySize[ent] = entSize;
                        m_oriMousePosition[ent] = pointY;
                        m_zoomEntityWeight[ent] = glm::vec3(0.f,1.f,1.f);
                        m_isNowMovingEntity[ent] = true;
                    }
                    else{
                        m_isNowMovingEntity.erase(ent);
                    }
                    m_zoomEntityWeight[ent] *= glm::vec3(entSize.x>0?1.f:-1.f,entSize.y>0?1.f:-1.f,1.f);

                } else {
                    // Scaling 用投影量計算增加的數值
                    const auto AB = m_oriMousePosition[ent] - m_oriEntityPosition[ent];
                    const auto AC = glm::vec3(mousePosInWorld, 0.f) - m_oriEntityPosition[ent];
                    auto ADcoefficient = ((AB.x*AC.x+AB.y*AC.y)/std::abs(AB.x*AB.x+AB.y*AB.y));
                    auto AD = glm::vec2(ADcoefficient*AB.x-AB.x, ADcoefficient*AB.y-AB.y);
                    auto addLen = sqrt(AD.x*AD.x+AD.y*AD.y);
                    addLen = addLen*((AB.x*AD.x+AB.y*AD.y)<0?-1:1) ;
                    entSize = m_oriEntitySize[ent] + glm::vec3(addLen*2, addLen*2,0.f)*m_zoomEntityWeight[ent];

                };
            }
            break;
        }
        case GizmoMode::RotationMode:
        {
            for (auto ent : m_entitySet)
            {
                const auto &entPosition = ent.getComponent<TransformComponent>().translate;
                const auto &entSize = ent.getComponent<TransformComponent>().scale;
                auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                const auto bound = BoundingBox2D::CalculateBoundingBox2D(entPosition, entSize, entRotate);
                const glm::vec3 boundPos(bound.getPosition(), 0.f);
                const glm::vec3 boundSize(bound.getScale(), 0.f);

                // The entity is not moving but selected
                if (!m_isNowMovingEntity[ent]) {

                    if (Math::AABB2DPoint(boundPos, boundSize, mousePosInWorld)) {
                        m_isNowMovingEntity[ent] = true;
                        m_oriEntityRotate[ent] = entRotate;
                        m_oriMouseRotate[ent] =
                                std::atan2(mousePosInWorld.y - boundPos.y, mousePosInWorld.x - boundPos.x) * 180 / M_PI;
                    }
                    else{
                        m_isNowMovingEntity.erase(ent);
                    }

                } else {
                    auto nowRotate = std::atan2(mousePosInWorld.y - boundPos.y, mousePosInWorld.x - boundPos.x) * 180 / M_PI;
                    entRotate = m_oriEntityRotate[ent] + (nowRotate - m_oriMouseRotate[ent]);
                };
            }
            break;
        }
    }
}

void Gizmo::onUpdate()
{
    BoundingBox2D mouseBound = BoundingBox2D::CalculateBoundingFromPoint(m_mousePosBegin, m_mousePosEnd);
    if (mouseBound.getScale().x > .001f && mouseBound.getScale().y > .001f)
    {
        Renderer2D::DrawQuad(mouseBound.getPosition(), mouseBound.getScale(), glm::vec4(0.f, 0.f, .3f, .3f));
    }

    // Early out if there is no any entity.
    if (m_entitySet.empty())
        return;

    // Force set to Move Mode when number > 1
    if (m_entitySet.size() > 1 && m_gizmoMode != GizmoMode::MoveMode)
        setGizmoMode(GizmoMode::MoveMode);

    // Calculate the new bound
    auto [boundPos, boundSize] = calculateNewBoundVec2();

    switch (m_gizmoMode)
    {
        case GizmoMode::MoveMode:
        {
            for (auto &ent : m_entitySet)
            {
                const auto &entPos = ent.getComponent<TransformComponent>().translate;
                const auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                // Get bounding box of evenry entity and draw it
                auto bound = BoundingBox2D::CalculateBoundingBox2D(entPos, entSize, entRotate);
                Renderer2D::DrawRect(bound.getPosition(), bound.getScale(), glm::vec4(0.8f));
            }

            // Draw the bounding box outside
            Renderer2D::DrawRect(boundPos, boundSize, glm::vec4(1.f));

            auto moveQuadPos2 = glm::vec2(boundPos + m_clickSize2 * glm::vec2(.5f));
            Renderer2D::DrawQuad(moveQuadPos2, m_clickSize2, colorY);
            auto moveQuadPos3 = glm::vec3(moveQuadPos2, 2.f);
            Renderer2D::DrawQuad(moveQuadPos3, m_clickSize2, colorY);

            auto PointX = boundPos + boundSize * glm::vec2(.5f, 0.f) + m_clickSize2 * glm::vec2(3.f, 0.f);
            auto PointY = boundPos + boundSize * glm::vec2(0.f, .5f) + m_clickSize2 * glm::vec2(0.f, 3.f);
            Renderer2D::DrawFgLine(boundPos, PointX, glm::vec4(0.f, 0.f, 1.f, 1.f));
            Renderer2D::DrawTriangle(PointX, PointX + m_clickSize2 * glm::vec2(-1.f, .5f),
                                     PointX + m_clickSize2 * glm::vec2(-1.f, -.5f), colorB);
            Renderer2D::DrawFgLine(boundPos, PointY, glm::vec4(1.f, 0.f, 0.f, 1.f));
            Renderer2D::DrawTriangle(PointY, PointY + m_clickSize2 * glm::vec2(.5f, -1.f),
                                     PointY + m_clickSize2 * glm::vec2(-.5f, -1.f), colorR);

            break;
        }
        case GizmoMode::ZoomMode:
        {
            for (auto &ent : m_entitySet)
            {
                const auto &entPos = ent.getComponent<TransformComponent>().translate;
                const auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                // Get bounding box
                auto bound = BoundingBox2D::CalculateBoundingBox2D(entPos, entSize, entRotate);

                Renderer2D::DrawRect(bound.getPosition(), bound.getScale(), glm::vec4(1.f));
                Renderer2D::DrawCircle(bound.getPosition() + bound.getScale() * glm::vec2(0.5f, 0.5f), m_clickSize2.x,
                                       colorY);
                Renderer2D::DrawQuad(bound.getPosition() + bound.getScale() * glm::vec2(0.5f, 0.0f), m_clickSize2, colorY);
                Renderer2D::DrawCircle(bound.getPosition() + bound.getScale() * glm::vec2(0.5f, -0.5f), m_clickSize2.x,
                                       colorY);
                Renderer2D::DrawQuad(bound.getPosition() + bound.getScale() * glm::vec2(0.0f, -0.5f), m_clickSize2, colorY);
                Renderer2D::DrawCircle(bound.getPosition() + bound.getScale() * glm::vec2(-0.5f, -0.5f), m_clickSize2.x,
                                       colorY);
                Renderer2D::DrawQuad(bound.getPosition() + bound.getScale() * glm::vec2(-0.5f, 0.0f), m_clickSize2, colorY);
                Renderer2D::DrawCircle(bound.getPosition() + bound.getScale() * glm::vec2(-0.5f, 0.5f), m_clickSize2.x,
                                       colorY);
                Renderer2D::DrawQuad(bound.getPosition() + bound.getScale() * glm::vec2(0.0f, 0.5f), m_clickSize2, colorY);
            }
            break;
        }
        case GizmoMode::ScaleMode:
        {
            for (auto &ent : m_entitySet)
            {
                const auto &entPos = ent.getComponent<TransformComponent>().translate;
                const auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;

                const auto tmpX = glm::vec3(entSize.x * 0.5, 0.f, 1.f) + m_clickSize3 * glm::vec3(3.f, 0.f, 1.f);
                const auto tmpY = glm::vec3(0.f, entSize.y * 0.5, 1.f) + m_clickSize3 * glm::vec3(0.f, 3.f, 1.f);
                const auto sinR = std::sin(glm::radians(entRotate));
                const auto cosR = std::cos(glm::radians(entRotate));
                const auto pointX = entPos + glm::vec3(tmpX.x * cosR - tmpX.y * sinR, tmpX.x * sinR + tmpX.y * cosR, 0.f);
                const auto pointY = entPos + glm::vec3(tmpY.x * cosR - tmpY.y * sinR, tmpY.x * sinR + tmpY.y * cosR, 0.f);

                Renderer2D::DrawFgLine(entPos, pointX, glm::vec4(0.f, 0.f, 1.f, 1.f));
                Renderer2D::DrawRotatedQuad(pointX, m_clickSize2, colorY, entRotate);
                Renderer2D::DrawFgLine(entPos, pointY, glm::vec4(1.f, 0.f, 0.f, 1.f));
                Renderer2D::DrawRotatedQuad(pointY, m_clickSize2, colorY, entRotate);
            }
            break;
        }
        case GizmoMode::RotationMode:
        {
            for (auto &ent : m_entitySet)
            {
                const auto &entPos = ent.getComponent<TransformComponent>().translate;
                const auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;
                // Get bounding box
                auto bound = BoundingBox2D::CalculateBoundingBox2D(entPos, entSize, entRotate);

                Renderer2D::DrawCircleLine(entPos, std::max(entSize.x, entSize.y));
                Renderer2D::DrawCircle(entPos, m_clickSize2.x, glm::vec4(0, 0.39, 0.79, 1));

                const auto tmpPoint = glm::vec3(0.f, (double)std::max(entSize.x, entSize.y) * 0.7, 1.f);
                const auto sinR = std::sin(glm::radians(entRotate));
                const auto cosR = std::cos(glm::radians(entRotate));
                Renderer2D::DrawFgLine(bound.getPosition(),
                                       glm::vec3(bound.getPosition(), 0.f) +
                                                glm::vec3(tmpPoint.x * cosR - tmpPoint.y * sinR,
                                                          tmpPoint.x * sinR + tmpPoint.y * cosR,
                                                          0.f),
                                      glm::vec4(1.f, 0.f, 0.f, 1.f));
            }
            break;
        }
    }
}

bool Gizmo::isMovingEntity()
{
    return !m_isNowMovingEntity.empty();
}

bool Gizmo::isMouseOnGizmo(glm::vec2 mousePosInWorld)
{
    // Calculate the new bound
    glm::vec3 boundPos(0.f);
    glm::vec3 boundSize(0.f);
    auto tmp = calculateEntitySetBound();
    boundPos = glm::vec3(tmp.getPosition(), 0.f);
    boundSize = glm::vec3(tmp.getScale(), 0.f);

    switch (m_gizmoMode)
    {
        case GizmoMode::MoveMode:
        {
            if(Math::AABB2DPoint(boundPos+m_clickSize3*glm::vec3(.5f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3(.5f,0.f, 1.f)+m_clickSize3*glm::vec3(1.5f,0.f,1.f), boundSize*glm::vec3(.5f,0.f,1.f) + m_clickSize3*glm::vec3(3.f,1.f,1.f), mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3(0.f,.5f, 1.f)+m_clickSize3*glm::vec3(0.f,1.5f,1.f), boundSize*glm::vec3(0.f,.5f,1.f) + m_clickSize3*glm::vec3(1.f,3.f,1.f), mousePosInWorld))
            {
                return true;
            }
            break;
        }
        case GizmoMode::ZoomMode:
        {
            if(Math::AABB2DPoint(boundPos+boundSize*glm::vec3( 0.5f,  0.5f, 1.f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3( 0.5f,  0.0f, 1.f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3( 0.5f, -0.5f, 1.f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3( 0.0f, -0.5f, 1.f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3(-0.5f, -0.5f, 1.f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3(-0.5f,  0.0f, 1.f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3(-0.5f,  0.5f, 1.f), m_clickSize3, mousePosInWorld) ||
               Math::AABB2DPoint(boundPos+boundSize*glm::vec3( 0.0f,  0.5f, 1.f), m_clickSize3, mousePosInWorld) )
            {
                return true;
            }
            break;
        }
        case GizmoMode::ScaleMode:
        {
            for(auto &ent : m_entitySet)
            {
                const auto &entPos = ent.getComponent<TransformComponent>().translate;
                const auto &entSize = ent.getComponent<TransformComponent>().scale;
                const auto &entRotate = ent.getComponent<TransformComponent>().rotate;

                const auto tmpX = glm::vec3(entSize.x*0.5,0.f,1.f)+m_clickSize3*glm::vec3(3.f, 0.f,1.f);
                const auto tmpY = glm::vec3(0.f,entSize.y*0.5,1.f)+m_clickSize3*glm::vec3(0.f, 3.f,1.f);
                const auto sinR = std::sin(glm::radians(entRotate));
                const auto cosR = std::cos(glm::radians(entRotate));
                const auto pointX = entPos + glm::vec3(tmpX.x*cosR-tmpX.y*sinR, tmpX.x*sinR+tmpX.y*cosR, 0.f);
                const auto pointY = entPos + glm::vec3(tmpY.x*cosR-tmpY.y*sinR, tmpY.x*sinR+tmpY.y*cosR, 0.f);

                if(Math::AABB2DPoint(pointX,m_clickSize3*glm::vec3(1.5f, 1.5f, 1.f), mousePosInWorld) ||
                   Math::AABB2DPoint(pointY,m_clickSize3*glm::vec3(1.5f, 1.5f, 1.f), mousePosInWorld))
                {
                    return true;
                }

            }
            break;
        }
        case GizmoMode::RotationMode:
        {
            if( Math::AABB2DPoint(boundPos, m_clickSize3, mousePosInWorld))
            {
                return true;
            }
            break;
        }
    }

    return false;
}

void Gizmo::setGizmoMode(GizmoMode mode)
{
    m_gizmoMode = mode;
}

void Gizmo::setMousePosBegin(glm::vec2 pos)
{
    m_mousePosBegin = pos;
}

void Gizmo::setMousePosEnd(glm::vec2 pos)
{
    m_mousePosEnd = pos;
}

void Gizmo::setClickSize(glm::vec2 size)
{
    m_clickSize2 = size;
    m_clickSize3 = glm::vec3(size,0.f);
}

void Gizmo::setClickSize(glm::vec3 size)
{
    m_clickSize2 = size;
    m_clickSize3 = size;
}

void Gizmo::setSelectedEntity(std::set<Entity> &ent)
{
    m_entitySet.clear();
    m_entitySet = ent;
}

BoundingBox2D Gizmo::getMouseBound()
{
    return BoundingBox2D::CalculateBoundingFromPoint(m_mousePosBegin, m_mousePosEnd);
}

BoundingBox2D Gizmo::calculateEntitySetBound()
{
    BoundingBox2D curBound;
    //
    for (auto ent : m_entitySet)
    {
        const auto &trans = ent.getComponent<TransformComponent>();
        const BoundingBox2D entBound = BoundingBox2D::CalculateBoundingBox2D(trans.translate, trans.scale, trans.rotate);
        curBound = BoundingBox2D::CombineBoundingBox2D(curBound, entBound);
    }
    //
    return curBound;
}

}
