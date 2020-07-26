#include <Rish/Renderer/Renderer.h>

#include <glm/glm.hpp>

struct SceneData
{
    glm::mat4 ViewProjectionMatrix{0.f};
};

static SceneData s_sceneData;

void rl::Renderer::BeginScene(OrthographicCamera &camera)
{
    s_sceneData.ViewProjectionMatrix = camera.getViewProjectionMatrix();
}

void rl::Renderer::EndScene()
{
}

void rl::Renderer::Submit(const std::shared_ptr<Shader> &shader, const std::shared_ptr<VertexArray> &vertexArray)
{
    shader->bind();
    shader->setMat4("u_ViewProjection", s_sceneData.ViewProjectionMatrix);
    //
    vertexArray->bind();
    RenderCommand::DrawElement(vertexArray);
    // TODO: check the reason that this must unbind
    vertexArray->unbind();
    shader->unbind();
}
