#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/VertexArray.h>

#include <glm/glm.hpp>

namespace rl {

/**
 * @brief Renderer Commands
 */
class RL_API RenderCommand
{
public:
    /**
     * @brief Init Command
     */
    static void Init();

    /**
     * @brief Set viewport
     * @details https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glViewport.xhtml
     * @param x lower left corner of the viewport rectangle
     * @param y lower left corner of the viewport rectangle
     * @param width
     * @param height
     */
    static void SetViewPort(float x, float y, float width, float height);

    /**
     * @brief Set Clear Color
     * @param color Color
     */
    static void SetClearColor(const glm::vec4 &color);

    /**
     * @brief Clear the render target
     */
    static void Clear();

    /**
     * @brief Draw By IndexBuffer
     * @param vertexArray Vertex Array
     */
    static void DrawElement(const Ref <VertexArray> &vertexArray, uint32_t indexCount=0);
};

}
