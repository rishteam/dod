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

    Ref<Framebuffer> m_framebuffer;
    Ref<Framebuffer> m_framebufferSecond;
    Ref<Texture2D> gray_pic;
};
