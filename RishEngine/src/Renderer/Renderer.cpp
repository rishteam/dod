#include <Rish/Renderer/Renderer.h>

struct SceneData
{
    glm::mat4 ViewProjectionMatrix{0.f};
};
static SceneData s_sceneData;

void rl::Renderer::Init()
{
    RenderCommand::Init();
}

void rl::Renderer::BeginScene(OrthographicCamera &camera)
{
    s_sceneData.ViewProjectionMatrix = camera.getViewProjectionMatrix();
}

void rl::Renderer::EndScene()
{
}

void rl::Renderer::Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray, const glm::mat4 &transform)
{
    shader->bind();
    shader->setMat4("u_ViewProjection", s_sceneData.ViewProjectionMatrix);
    shader->setMat4("u_Model", transform);
    //
    vertexArray->bind();
    RenderCommand::DrawElement(vertexArray);
    // TODO: check the reason that this must unbind
    vertexArray->unbind();
    shader->unbind();
}

void rl::Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewPort(0, 0, (float)width, (float)height);
}
