#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/RendererCommand.h>

namespace rl {

/**
 * @brief Renderer class
 */
class RL_API Renderer
{
public:
    /**
     * @brief Begin Scene
     */
    static void BeginScene();

    /**
     * @brief End Scene
     */
    static void EndScene();

    /**
     * @brief Submit Vertex Array
     * @param vertexArray Vertex Array
     */
    static void Submit(const std::shared_ptr<VertexArray> &vertexArray);
};

} // end of namespace rl