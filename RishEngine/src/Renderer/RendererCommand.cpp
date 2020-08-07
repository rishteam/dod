#include <glad/glad.h>

#include <Rish/Core/Core.h>
#include <Rish/Renderer/RendererCommand.h>

void APIENTRY OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         RL_CORE_CRITICAL(message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:       RL_CORE_ERROR(message); return;
        case GL_DEBUG_SEVERITY_LOW:          RL_CORE_WARN(message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: RL_CORE_TRACE(message); return;
    }

    RL_CORE_ASSERT(false, "Unknown severity level!");
}

void rl::RenderCommand::Init()
{
#ifdef RL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void rl::RenderCommand::SetClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void rl::RenderCommand::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void rl::RenderCommand::DrawElement(const Ref <VertexArray> &vertexArray, uint32_t indexCount)
{
    indexCount = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void rl::RenderCommand::SetViewPort(float x, float y, float width, float height)
{
    glViewport((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height);
}
