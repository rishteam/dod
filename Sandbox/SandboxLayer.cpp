#include "SandboxLayer.h"

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    gray_pic = Texture2D::LoadTextureVFS("/texture/dev_128_gr_032x.jpg");

    m_framebuffer = Framebuffer::Create({1024, 768, 1});
    m_framebufferSecond = Framebuffer::Create({1024, 768, 1});
}

void ExampleSandboxLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/ExampleSandboxLayer.ini");
}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/ExampleSandboxLayer.ini");
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);

    m_rotate += 10.f * dt.asSeconds();

    m_framebuffer->bind();
    RenderCommand::SetClearColor({0.f, 0.f, 0.f, 1.f});
    RenderCommand::Clear(RenderCommand::ClearBufferTarget::ColorBuffer | RenderCommand::ClearBufferTarget::DepthBuffer);
    Renderer2D::BeginScene(m_cameraController.getCamera(), true);
    {
        for(int i = 0; i < 10000; i++)
            Renderer2D::DrawQuad({i, 0.f, i + 0.1f}, {1.f, 1.f}, gray_pic, glm::vec4(1.f), 1.f);
    }
    Renderer2D::EndScene();
    m_framebuffer->unbind();

    m_framebufferSecond->bind();
    RenderCommand::SetClearColor({0.f, 0.f, 0.f, 1.f});
    RenderCommand::Clear(RenderCommand::ClearBufferTarget::ColorBuffer | RenderCommand::ClearBufferTarget::DepthBuffer);
    Renderer2D::BeginScene(m_cameraController.getCamera(), true);
    {
        for(int i = 0; i < 100; i++)
            for(int j = 0; j < 100; j++)
                Renderer2D::DrawQuad({i, i, j}, {0.9f, 0.9f}, gray_pic, glm::vec4(1.f), 1.f);
    }
    Renderer2D::EndScene();
    m_framebufferSecond->unbind();
}

void ExampleSandboxLayer::onImGuiRender()
{
    ImGui::Begin("A");
    ImGui::Image(m_framebuffer->getColorAttachmentRendererID(),
                 ImVec2{static_cast<float>(m_framebuffer->getWidth()), static_cast<float>(m_framebuffer->getHeight())}
                 , {0, 0}, {1, -1});
    ImGui::End();

    ImGui::Begin("B");
    ImGui::Image(m_framebufferSecond->getColorAttachmentRendererID(),
                 ImVec2{static_cast<float>(m_framebufferSecond->getWidth()), static_cast<float>(m_framebufferSecond->getHeight())}
            , {0, 0}, {1, -1});
    ImGui::End();
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
