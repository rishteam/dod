#pragma once

#include <Rish/rlpch.h>
#include <glm/glm.hpp>
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
     * @brief Initialize the Renderer
     */
    static void Init();

    /**
     * @brief
     * @param width
     * @param height
     */
    static void OnWindowResize(uint32_t width, uint32_t height);

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
    static void Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray, const glm::mat4 &transform=glm::mat4(1.f));
};

} // end of namespace rl