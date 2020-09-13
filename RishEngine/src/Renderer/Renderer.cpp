#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>

namespace rl {

struct SceneData
{
    glm::mat4 ViewProjectionMatrix{0.f};
};
static SceneData s_sceneData;

void Renderer::Init()
{
    RL_PROFILE_FUNCTION();

    RenderCommand::Init();
    Renderer2D::Init();
}

void Renderer::Shutdown()
{
    RL_PROFILE_FUNCTION();

    Renderer2D::Shutdown();
}

void Renderer::BeginScene(OrthographicCamera &camera)
{
    s_sceneData.ViewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray, const glm::mat4 &transform)
{
    shader->bind();
    shader->setMat4("u_ViewProjection", s_sceneData.ViewProjectionMatrix);
    shader->setMat4("u_Model", transform);
    //
    vertexArray->bind();
    RenderCommand::DrawElement(DrawTriangles, vertexArray, 0);
    // TODO: check the reason that this must unbind
    vertexArray->unbind();
    shader->unbind();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewPort(0, 0, (float) width, (float) height);
}

} // namespace rl