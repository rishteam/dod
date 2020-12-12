#include <Rish/rlpch.h>
#include <glad/glad.h>

#include <Rish/Renderer/RendererCommand.h>
#include <Rish/Renderer/VertexArray.h>
#include <Rish/Renderer/Buffer.h>

void APIENTRY OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         RL_CORE_CRITICAL("[OpenGL] {}", message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:       RL_CORE_ERROR("[OpenGL] {}", message); return;
        case GL_DEBUG_SEVERITY_LOW:          RL_CORE_WARN("[OpenGL] {}", message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: RL_CORE_TRACE("[OpenGL] {}", message); return;
    }

    RL_CORE_ASSERT(false, "Unknown severity level!");
}

namespace rl {

void RenderCommand::Init()
{
#ifdef RL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

    SetBlendMode(true);
    SetBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);

//    SetDepthTest(false);
}

void RenderCommand::SetClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void RenderCommand::Clear(uint32_t clearTarget)
{
    GLbitfield mask{};
    if(clearTarget & ClearBufferTarget::ColorBuffer)
        mask |= GL_COLOR_BUFFER_BIT;
    if(clearTarget & ClearBufferTarget::DepthBuffer)
        mask |= GL_DEPTH_BUFFER_BIT;
    if(clearTarget & ClearBufferTarget::StencilBuffer)
        mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
}

void RenderCommand::DrawElement(DrawType drawType, const Ref <VertexArray> &vertexArray, uint32_t indexCount, bool depthTest)
{
    indexCount = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();

    GLenum type = drawType == DrawTriangles ? GL_TRIANGLES : GL_LINES;

    bool depth = GetDepthTest();
    //
    SetDepthTest(depthTest);
    glDrawElements(type, indexCount, GL_UNSIGNED_INT, nullptr);
    SetDepthTest(depth);
}

void RenderCommand::SetViewPort(float x, float y, float width, float height)
{
    glViewport((GLint) x, (GLint) y, (GLsizei) width, (GLsizei) height);
}

void RenderCommand::ClearStates()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderCommand::SetDepthTest(bool state)
{
    if(state)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

bool RenderCommand::GetDepthTest()
{
    GLboolean depth{};
    glGetBooleanv(GL_DEPTH_TEST, &depth);
    return depth;
}

void RenderCommand::SetDepthFunc(DepthFunc option)
{
    GLenum opt{};
    switch (option)
    {
        case DepthFunc::Less:
            opt = GL_LESS;
        break;
        case DepthFunc::LessEqual:
            opt = GL_LEQUAL;
        break;
        case DepthFunc::Equal:
            opt = GL_EQUAL;
        break;
    }
    glDepthFunc(opt);
}

void RenderCommand::SetLineThickness(float t)
{
    glLineWidth(t);
}

void RenderCommand::SetLineSmooth(bool state)
{
    if(state)
        glEnable(GL_LINE_SMOOTH);
    else
        glDisable(GL_LINE_SMOOTH);
}

void RenderCommand::SetBlendMode(bool state)
{
    if(state)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void RenderCommand::SetBlendFunc(BlendFactor src, BlendFactor dst)
{
    GLenum s{};
    switch (src)
    {
        case BlendFactor::Zero:
            s = GL_ZERO;
            break;
        case BlendFactor::One:
            s = GL_ONE;
            break;
        case BlendFactor::SrcColor:
            s = GL_SRC_COLOR;
            break;
        case BlendFactor::DstColor:
            s = GL_DST_COLOR;
            break;
        case BlendFactor::OneMinusSrcColor:
            s = GL_ONE_MINUS_SRC_COLOR;
            break;
        case BlendFactor::OneMinusDstColor:
            s = GL_ONE_MINUS_DST_COLOR;
            break;
        case BlendFactor::SrcAlpha:
            s = GL_SRC_ALPHA;
            break;
        case BlendFactor::DstAlpha:
            s = GL_DST_ALPHA;
            break;
        case BlendFactor::OneMinusSrcAlpha:
            s = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case BlendFactor::OneMinusDstAlpha:
            s = GL_ONE_MINUS_DST_ALPHA;
            break;
    }
    //
    GLenum d{};
    switch (dst)
    {
        case BlendFactor::Zero:
            d = GL_ZERO;
            break;
        case BlendFactor::One:
            d = GL_ONE;
            break;
        case BlendFactor::SrcColor:
            d = GL_SRC_COLOR;
            break;
        case BlendFactor::DstColor:
            d = GL_DST_COLOR;
            break;
        case BlendFactor::OneMinusSrcColor:
            d = GL_ONE_MINUS_SRC_COLOR;
            break;
        case BlendFactor::OneMinusDstColor:
            d = GL_ONE_MINUS_DST_COLOR;
            break;
        case BlendFactor::SrcAlpha:
            d = GL_SRC_ALPHA;
            break;
        case BlendFactor::DstAlpha:
            d = GL_DST_ALPHA;
            break;
        case BlendFactor::OneMinusSrcAlpha:
            d = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case BlendFactor::OneMinusDstAlpha:
            d = GL_ONE_MINUS_DST_ALPHA;
            break;
    }
    //
    glBlendFunc(s, d);
}

void RenderCommand::SetStencilTest(bool state)
{
    if(state)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
}

void RenderCommand::SetStencilFunc(StencilFuncFactor func, int ref, int mask)
{
    GLenum f {};

    switch(func)
    {
        case StencilFuncFactor::Less:
            f = GL_LESS;
            break;
        case StencilFuncFactor::Equal:
            f = GL_EQUAL;
            break;
        case StencilFuncFactor::Always:
            f = GL_ALWAYS;
            break;
    }

    glStencilFunc(f, ref, mask);
}

void RenderCommand::SetStencilOp(StencilOpFactor sfail, StencilOpFactor dpfail, StencilOpFactor dppass)
{
    auto setStencilOp = [&](StencilOpFactor factor) -> GLenum
    {
        switch (factor)
        {
            case StencilOpFactor::Keep:
                return GL_KEEP;
                break;
            case StencilOpFactor::Zero:
                return GL_ZERO;
                break;
            case StencilOpFactor::Replace:
                return GL_REPLACE;
                break;
        }
    };

    GLenum s    = setStencilOp(sfail);
    GLenum dpf  = setStencilOp(dpfail);
    GLenum dpp  = setStencilOp(dppass);

    glStencilOp(s, dpf, dpp);
}

void RenderCommand::SetColorMask(bool r, bool g, bool b, bool a)
{
    glColorMask(r, g, b, a);
}

}
