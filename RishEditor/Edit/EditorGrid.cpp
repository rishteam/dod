#include "EditorGrid.h"

#include <Rish/Renderer/Renderer2D.h>

#include <Rish/ImGui/ImGui.h>

namespace rl {

EditorGrid::EditorGrid()
{
}

void EditorGrid::onUpdate(bool show)
{
    const auto & pos = m_cameraController->getPosition();
    // Get the camera bound with ref to current camera pos
    m_currentBound = m_cameraController->getBounds();

    float nowMinBound = std::ceil(std::min(m_currentBound.right, m_currentBound.top));
    // Less than the previous limit
    if(nowMinBound <= preLimit)
    {
        // go back
        currentOffset /= 10.f;
        nextOffset /= 10.f;
        //
        limit /= 10.f;
        preLimit /= 10.f;
    }

    // Calculate the color value by remaining value
    float remainVal = (limit - nowMinBound) / limit;
    glm::vec4 cur = glm::lerp(endColor, startColor, std::max(0.f, remainVal));

    if(remainVal < 0.f)
    {
        // Go to next offset
        currentOffset *= 10.f;
        nextOffset *= 10.f;
        // Set the limits
        preLimit = limit;
        limit *= 10.f;
    }

    m_currentBound.left += pos.x;
    m_currentBound.right += pos.x;
    m_currentBound.top += pos.y;
    m_currentBound.bottom += pos.y;

    // Draw only current and next level
    if(show)
    {
        drawGrid(currentOffset, cur);
        drawGrid(nextOffset, startColor);
        //
        drawGrid(0, glm::vec4{0.8f, 0.f, 0.f, 1.0f});
    }
}

void EditorGrid::onImGuiRender()
{
    ImGui::Begin("EditorGrid", nullptr);
    ImGui::ColorEdit4("Start Color", glm::value_ptr(startColor), ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("End Color", glm::value_ptr(endColor), ImGuiColorEditFlags_Float);
    //
    ImGui::Text("left=%f right=%f bottom=%f top=%f", m_currentBound.left, m_currentBound.right, m_currentBound.bottom, m_currentBound.top);
    //
    float nowLevel = std::ceil(std::min(m_currentBound.right, m_currentBound.top));
    ImGui::Text("lev = %f", nowLevel);
    ImGui::Text("lerp val = %f", (limit-nowLevel)/limit);
    ImGui::DragFloat("limit", &limit, 0.01f);
    ImGui::Text("currentOffset = %f | nextOffset = %f", currentOffset, nextOffset);
    ImGui::Text("limit = %f | preLimit = %f", limit, preLimit);
    ImGui::End();
}

void EditorGrid::drawGrid(float offset, const glm::vec4 &color)
{
    auto &bound = m_currentBound;

    if(offset == 0.f)
    {
        Renderer2D::DrawFgLine({0.f, bound.bottom, -1.f}, {0.f, bound.top, -1.f}, color);
        Renderer2D::DrawFgLine({bound.left, 0.f, -1.f}, {bound.right, 0.f, -1.f}, color);
        return;
    }

    // straight lines
    // left
    for(float i = 0.f; i >= std::floor(bound.left); i -= offset)
        Renderer2D::DrawFgLine({i, bound.bottom, -1}, {i, bound.top, -1}, color);
    // right
    for(float i = 0.f; i < std::ceil(bound.right); i += offset)
        Renderer2D::DrawFgLine({i, bound.bottom, -1}, {i, bound.top, -1}, color);

    // horizontal lines
    // bottom
    for(float i = 0.f; i >= std::floor(bound.bottom); i -= offset)
        Renderer2D::DrawFgLine({bound.left, i, -1}, {bound.right, i, -1}, color);
    // top
    for(float i = 0.f; i < std::ceil(bound.top); i += offset)
        Renderer2D::DrawFgLine({bound.left, i, -1}, {bound.right, i, -1}, color);
}

float EditorGrid::getOffset(){
    return (m_currentBound.getWidth()+m_currentBound.getHeight())/10;
}

}