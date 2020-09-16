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

    Entity ent = m_scene->createEntity();

    ent.addComponent<RenderComponent>("/shader/textured.vert", "/shader/textured.frag", glm::vec4{1,1,1,1});
    ent.addComponent<Physics2D>();
    ent.addComponent<BoxCollider2D>();
}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("Sandbox/imgui.ini");
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);
    //
    Renderer2D::BeginScene(m_cameraController.getCamera());
    m_scene->onUpdate(m_cameraController.getCamera(), dt);
    Renderer2D::EndScene();
}

void ExampleSandboxLayer::onImGuiRender()
{

}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
