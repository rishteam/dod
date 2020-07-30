#include <glad/glad.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/RendererCommand.h>

void rl::RenderCommand::Init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
}

void rl::RenderCommand::SetClearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void rl::RenderCommand::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void rl::RenderCommand::DrawElement(const Ref<VertexArray> &vertexArray)
{
    glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
}

void rl::RenderCommand::SetViewPort(float x, float y, float width, float height)
{
    glViewport((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height);
}
