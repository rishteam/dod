#include "SandboxLayer.h"

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    m_bg = Texture2D::LoadTextureVFS("/texture/dev_128_gr_032x.jpg");

    auto path = m_bg->getPath();

    RL_INFO("{}", path);
    RL_INFO("{}", FileSystem::RelativePath(path));
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
        Renderer2D::DrawQuad({0.f, 0.f, 1.f}, {10.f, 10.f}, m_bg, glm::vec4(1.f), 5.f);
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
