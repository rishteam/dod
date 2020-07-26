#pragma once

#include <Rish/rlpch.h>
//
#include <Rish/Renderer/Shader.h>
#include <Rish/Renderer/RendererCommand.h>
//
#include <Rish/Renderer/Camera/OrthographicCamera.h>

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
    static void BeginScene(OrthographicCamera &camera);

    /**
     * @brief End Scene
     */
    static void EndScene();

    /**
     * @brief Submit Vertex Array
     * @param vertexArray Vertex Array
     */
    static void Submit(const std::shared_ptr<Shader> &shader, const std::shared_ptr<VertexArray> &vertexArray);
};

} // end of namespace rl