#include "ParticleTestLayer.h"

using rl::ParticleComponent, rl::ParticleSystem;

ParticleTestLayer::ParticleTestLayer()
    : Layer("ParticleTestLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    m_scene = rl::MakeRef<rl::Scene>();
    m_scene->m_debugCamera = true;
    m_scene->setSceneState(rl::Scene::SceneState::Play);
}

ParticleTestLayer::~ParticleTestLayer()
{

}

void ParticleTestLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/ParticleTestLayer.ini");

    auto particleEntity = m_scene->createEntity("particle");
    particleEntity.addComponent<ParticleComponent>();
}

void ParticleTestLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/ParticleTestLayer.ini");
}

void ParticleTestLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);

    RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::One);
    Renderer2D::BeginScene(m_cameraController.getCamera(), true);
    {
        ParticleSystem::onUpdate(dt);
        ParticleSystem::onRender();
    }
    Renderer2D::EndScene();
    RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
}

void ParticleTestLayer::onImGuiRender()
{
    m_cameraController.onImGuiRender();
    m_scene->onImGuiRender();
}

void ParticleTestLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
