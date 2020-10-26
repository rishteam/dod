#include "SandboxLayer.h"

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    m_bg = Texture2D::LoadTextureVFS("/texture/bg.jpg");
    m_player = Texture2D::LoadTextureVFS("/texture/1.png");
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

    Renderer2D::BeginScene(m_cameraController.getCamera(), true);
    {
//        Renderer2D::DrawQuad({0.f, 0.f, 1.f}, {1.f, 1.f}, m_player);

        Renderer2D::DrawFgRect({0.f, 0.f}, {1.f, 1.f}, glm::vec4(1.f));
        Renderer2D::DrawCircleLine({0.f, 0.f}, 1.f, glm::vec4(1.f));


        Renderer2D::DrawTriangle({0.f, 2.f, 0.f}, {2.f, 0.f, 0.f}, {-2.f, 0.f, 0.f}, glm::vec4(1.f));
    }
    Renderer2D::EndScene();
}

void ExampleSandboxLayer::onImGuiRender()
{
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
