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
    static void DrawElement(const Ref<VertexArray> &vertexArray);
};

}
