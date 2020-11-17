#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

#include <Rish/Animation/Animation.h>
#include <Rish/Animation/Component.h>
//#include "../RishEditor/Panels/Animation/AnimationEditor.h"

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

    Ref<Texture2D> m_testTexture;
    Animation m_testAni;
    Animation2DComponent m_component;

//    AnimationEditor m_aniEditor;
};
