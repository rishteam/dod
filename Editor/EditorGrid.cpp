#include "EditorGrid.h"

#include <Rish/Renderer/Renderer2D.h>

namespace rl {

void EditorGrid::onUpdate(const OrthographicCameraController &cameraController)
{
    const auto & pos = cameraController.getPosition();
    m_currentBound = cameraController.getBounds();
    m_currentBound.left += pos.x;
    m_currentBound.right += pos.x;
    m_currentBound.top += pos.y;
    m_currentBound.bottom += pos.y;

    int nowLevel = (int)std::ceil(std::max(m_currentBound.right, m_currentBound.bottom)) / 10;
    RL_INFO("lev={} | {} {} {} {}", nowLevel, m_currentBound.left, m_currentBound.right, m_currentBound.bottom, m_currentBound.top);

    drawLines(1, glm::vec4{0.5f});
    drawLines(10, glm::vec4{0.8f});
    drawLines(0, glm::vec4{1.f, 0.f, 0.f,  1.0f});
}

void EditorGrid::drawLines(float offset, const glm::vec4 &color)
{
    auto &bound = m_currentBound;

    if(offset == 0.f)
    {
        Renderer2D::DrawLine({0.f, bound.bottom, -1.f}, {0.f, bound.top, -1.f}, color);
        Renderer2D::DrawLine({bound.left, 0.f, -1.f}, {bound.right, 0.f, -1.f}, color);
        return;
    }

    // straight lines
    // left
    for(float i = 0.f; i >= std::ceil(bound.left); i -= offset)
        Renderer2D::DrawLine({i, bound.bottom, -1}, {i, bound.top, -1}, color);
    // right
    for(float i = 0.f; i < std::ceil(bound.right); i += offset)
        Renderer2D::DrawLine({i, bound.bottom, -1}, {i, bound.top, -1}, color);

    // horizontal lines
    // bottom
    for(float i = 0.f; i >= std::ceil(bound.bottom); i -= offset)
        Renderer2D::DrawLine({bound.left, i, -1}, {bound.right, i, -1}, color);
    // top
    for(float i = 0.f; i < std::ceil(bound.top); i += offset)
        Renderer2D::DrawLine({bound.left, i, -1}, {bound.right, i, -1}, color);
}

}