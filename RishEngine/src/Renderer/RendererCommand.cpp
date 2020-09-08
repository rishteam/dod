#include <glad/glad.h>

#include <Rish/Core/Core.h>
#include <Rish/Renderer/RendererCommand.h>

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SetDepthTest(true);
}

void RenderCommand::SetClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void RenderCommand::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderCommand::DrawElement(DrawType drawType, const Ref <VertexArray> &vertexArray, uint32_t indexCount)
{
    indexCount = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();

    GLenum type = drawType == DrawTriangles ? GL_TRIANGLES : GL_LINES;
    glDrawElements(type, indexCount, GL_UNSIGNED_INT, nullptr);
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

}
