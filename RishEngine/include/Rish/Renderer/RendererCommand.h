#pragma once

#include <Rish/rlpch.h>

#include <Rish/Renderer/fwd.h>

namespace rl {

/**
 * @brief Draw Premitive type
 */
enum DrawType
{
    DrawTriangles = 0, ///< Triangles
    DrawLines,         ///< Lines
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
     * @brief Get depth test
     */
    static bool GetDepthTest();

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
     * @brief Blend Function options
     * @details <https://www.cnblogs.com/ylwn817/archive/2012/09/07/2675285.html>
     */
    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        DstColor,
        OneMinusSrcColor,
        OneMinusDstColor,
        SrcAlpha,
        DstAlpha,
        OneMinusSrcAlpha,
        OneMinusDstAlpha
    };

    /**
     * @brief Stencil function option
     * @details <https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glStencilFunc.xhtml>
     */
    enum class StencilFuncFactor
    {
        Less,
        Equal,
        Always
    };

    /**
     * @brief Stencil function option
     * @details <https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glStencilOp.xhtml>
     */
    enum class StencilOpFactor
    {
        Keep,
        Zero,
        Replace
    };

    /**
     * @brief Turn on/off Blend
     * @param state 
     */
    static void SetBlendMode(bool state);

    /**
     * @brief Set Blend Function
     * @param src Source Factor
     * @param dst Destination Factor
     */
    static void SetBlendFunc(BlendFactor src, BlendFactor dst);


    static void SetStencilTest(bool state);

    static void SetStencilFunc(StencilFuncFactor func, int ref, int mask);

    static void SetStencilOp(StencilOpFactor sfail, StencilOpFactor dpfail, StencilOpFactor dppass);

    static void SetColorMask(bool r, bool g, bool b, bool a);

    /**
     * @brief Clear Mode
     * @details <http://docs.gl/gl4/glClear>
     */
    enum ClearBufferTarget
    {
        ColorBuffer   = BIT(0),
        DepthBuffer   = BIT(1),
        StencilBuffer = BIT(2)
    };

    /**
     * @brief Clear the render target
     */
    static void Clear(uint32_t clearTarget);

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
