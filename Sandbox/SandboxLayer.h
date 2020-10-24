#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

using namespace rl;

class ExampleSandboxLayer : public rl::Layer
{
public:
    ExampleSandboxLayer();

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event& event) override;

private:
    OrthographicCameraController m_cameraController;
    float m_rotate = 0.f;

    Ref<Texture2D> m_bg;
    Ref<Texture2D> m_player;
};
