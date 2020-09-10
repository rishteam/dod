#include "EditorGrid.h"

#include <Rish/Renderer/Renderer2D.h>

namespace rl {

void EditorGrid::onUpdate(const OrthographicCameraController &cameraController)
{
    m_cameraBound = cameraController.getBounds();

    int nowLevel = std::ceil(std::max(m_cameraBound.right, m_cameraBound.top)) / 10;
    RL_INFO("lev={} | {} {} {} {}", nowLevel, m_cameraBound.left, m_cameraBound.right, m_cameraBound.bottom, m_cameraBound.top);
    drawLines(1, glm::vec4{1.f});
    drawLines(10, glm::vec4{1.f, 0.f, 0.f,  1.f});
}

void EditorGrid::drawLines(int offset, const glm::vec4 &color)
{
    auto &bound = m_cameraBound;
    //
    // straight lines
    int i = std::floor(bound.left);
    while(i < std::ceil(bound.right))
    {
        Renderer2D::DrawLine({i, bound.bottom, -1}, {i, bound.top, -1}, color);
        i += offset;
    }
    // horizontal lines
    i = std::floor(bound.bottom);
    while(i < std::ceil(bound.top))
    {
        Renderer2D::DrawLine({bound.left, i, -1}, {bound.right, i, -1}, color);
        i += offset;
    }
}

}