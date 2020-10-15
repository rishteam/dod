#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

using rl::Ref, rl::MakeRef;
using rl::RigidBody2D;
using rl::Renderer2D, rl::OrthographicCameraController, rl::RenderCommand;
using rl::Application, rl::Scene;

class ParticleTestLayer : public rl::Layer
{
public:
    ParticleTestLayer();
    ~ParticleTestLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event &event) override;
private:
    Ref<Scene> m_scene;
    OrthographicCameraController m_cameraController;
};
