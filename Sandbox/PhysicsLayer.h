#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

using rl::Ref;
using rl::MakeRef;
using rl::RigidBody2D;
using rl::Renderer2D;
using rl::OrthographicCameraController;
using rl::Application;

class PhysicsLayer : public rl::Layer
{
public:
    PhysicsLayer();
    ~PhysicsLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event &event) override;

private:
    float x, y, w, h, m;
    Ref<rl::PhysicsWorld> m_world;
    OrthographicCameraController m_cameraController;
};
