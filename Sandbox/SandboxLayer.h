#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

using namespace rl;

class ExampleSandboxLayer : public rl::Layer
{
public:
    ExampleSandboxLayer();
    ~ExampleSandboxLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event& event) override;

private:
    bool onKeyPressEvent(KeyPressedEvent &event);
    //
    OrthographicCameraController m_cameraController;

    Ref<Texture2D> m_testTexture;
    Ref<Scene> m_scene;
};
