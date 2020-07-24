#include <Rish/Renderer/Renderer.h>


void rl::Renderer::BeginScene()
{
}

void rl::Renderer::EndScene()
{
}

void rl::Renderer::Submit(const std::shared_ptr<VertexArray> &vertexArray)
{
    vertexArray->bind();
    RenderCommand::DrawElement(vertexArray);
    vertexArray->unbind(); // TODO: check the reason that this must unbind
}
