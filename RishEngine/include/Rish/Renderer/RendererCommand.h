#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/VertexArray.h>

#include <glm/glm.hpp>

namespace rl {

enum DrawType
{
    DrawTriangles = 0,
    DrawLines,
    DrawTypeCount
};

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
     * @brief Set depth test
     * @param state true/false
     */
    static void SetDepthTest(bool state);

    /**
     * @brief Depth function option
     * @details <https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDepthFunc.xhtml>
     */
    enum class DepthFunc
    {
        Less,
        LessEqual,
        Equal
    };
    /**
     * @brief Set depth function
     * @param option
     */
    static void SetDepthFunc(DepthFunc option);

    /**
     * @brief Clear the render target
     */
    static void Clear();

    /**
     * @brief Reset GL3+ states
     */
    static void ClearStates();

    /**
     * @brief Draw By IndexBuffer
     * @param drawType Primitive type
     * @param vertexArray VA
     * @param indexCount Index count
     * @param depthTest Enable/Disable depth test
     */
    static void DrawElement(DrawType drawType, const Ref <VertexArray> &vertexArray, uint32_t indexCount=0, bool depthTest=false);

    static void SetLineThickness(float t);
    static void SetLineSmooth(bool state);
};

}
