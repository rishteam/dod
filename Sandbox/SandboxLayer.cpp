#include "SandboxLayer.h"

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio())
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    ImGui::LoadIniSettingsFromDisk("Sandbox/imgui.ini");
}

void ExampleSandboxLayer::onAttach()
{
    auto cur = rl::FileSystem::GetCurrentDirectoryPath();
    m_scene = rl::MakeRef<rl::Scene>();
    m_world = rl::MakeRef<rl::PhysicsWorld>(Vec2(0.0, -9.8), 800.0f, 600.0f);


}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("Sandbox/imgui.ini");
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);
    Renderer2D::BeginScene(m_cameraController.getCamera());
    Renderer2D::DrawQuad({0, 0}, {1 , 1}, {1, 1, 1, 1});
    Renderer2D::EndScene();
}

void ExampleSandboxLayer::onImGuiRender()
{

}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
