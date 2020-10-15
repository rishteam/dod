#include "SandboxLayer.h"
#include <Rish/Effect/Particle/ParticleSystem.h>

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
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

    auto particleEntity = m_scene->createEntity("particle");
    particleEntity.addComponent<ParticleComponent>();
}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("Sandbox/imgui.ini");
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);
    // TODO : BlendFunc
    RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::One);
    Renderer2D::BeginScene(m_cameraController.getCamera(), true);
//    m_world->timeStep = dt;

//    ParticleSystem::onUpdate(m_scene->m_registry, dt);
//    ParticleSystem::onRender(m_scene->m_registry, m_scene->getSceneState());

    Renderer2D::EndScene();
    RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
}

void ExampleSandboxLayer::onImGuiRender()
{
    m_cameraController.onImGuiRender();
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
