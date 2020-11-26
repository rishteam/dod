#include <LightLayer.h>


LightLayer::LightLayer()
    : Layer("LightLayer"), m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    RL_TRACE("Light Layer Construct");
    radius = 1000;
    strength = 20;
    color = glm::vec4(1, 1, 1, 1);
    lightPos = glm::vec3(0, 0, 0);
    texture = MakeRef<rl::Texture2D>("assets/texture/gardevoir.jpg", false);

    FramebufferSpecification spec{Application::Get().getWindow().getWidth(), Application::Get().getWindow().getHeight()};

    world = Framebuffer::Create(spec);
    light = Framebuffer::Create(spec);
}

LightLayer::~LightLayer()
{

}

void LightLayer::onAttach()
{
}

void LightLayer::onDetach()
{
}

void LightLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);
    world->bind();
    glm::vec2 viewPortSize = {Application::Get().getWindow().getWidth() / 2, Application::Get().getWindow().getHeight()/2};

    {
        RenderCommand::SetClearColor(glm::vec4(0, .0, .0, 1));
        RenderCommand::Clear(RenderCommand::ClearBufferTarget::ColorBuffer | RenderCommand::ClearBufferTarget::DepthBuffer);
        Renderer2D::BeginScene(m_cameraController.getCamera(), false);
        Renderer2D::DrawQuad({0, 0}, {2, 2}, texture);
        Renderer2D::EndScene();
    }
    world->unbind();

    {
        light->bind();
        RenderCommand::SetClearColor(glm::vec4(0, 0, 0, 0));
        RenderCommand::Clear(RenderCommand::ClearBufferTarget::ColorBuffer | RenderCommand::ClearBufferTarget::DepthBuffer);
        Renderer2D::BeginScene(m_cameraController.getCamera(), false);
        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
        Renderer2D::DrawQuad({0, 0}, {m_cameraController.getAspect() * m_cameraController.getZoom() * 2, m_cameraController.getZoom() * 2}, {0, 0, 0, 1});
        Renderer2D::DrawPointLight(lightPos, radius, strength, lightPos,
                                   viewPortSize,
                                   {Application::Get().getWindow().getWidth(), Application::Get().getWindow().getHeight()},
                                   m_cameraController.getZoom(), m_cameraController.getAspect(), color);
        Renderer2D::EndScene();
        light->unbind();
    }

    {
        world->bind();
        Renderer2D::CombineFramebuffer(world, light);
        world->unbind();
    }
}

void LightLayer::onImGuiRender()
{
    ImGui::Begin("Attribute");
    ImGui::ColorEdit4("Color", glm::value_ptr(color), ImGuiColorEditFlags_Float);
    ImGui::DragFloat("Radius", &radius, 1, 0, FLT_MAX, "%.2f");
    ImGui::DragFloat("Strength", &strength, 1, 0, FLT_MAX, "%.2f");
    ImGui::DragFloat("PosX", &lightPos.x, 0.01);
    ImGui::DragFloat("PosY", &lightPos.y, 0.01);
    ImGui::End();

    ImGui::Begin("world");
    uint32_t textureID = world->getColorAttachmentRendererID();
    ImGui::Image(textureID, {(float)world->getWidth(), (float)world->getHeight()}, {0, 0}, {1, -1});
    ImGui::End();

//    ImGui::SetNextWindowSize(ImVec2{1280, 720});
//    ImGui::Begin("Light", nullptr, ImGuiWindowFlags_NoResize);
//    textureID = light->getColorAttachmentRendererID();
//    ImGui::Image(textureID, {(float)light->getWidth(), (float)light->getHeight()}, {0, 0}, {1, -1});
//    ImGui::End();

}

void LightLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
