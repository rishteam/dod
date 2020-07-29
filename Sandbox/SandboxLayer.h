#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

class ExampleSandboxLayer : public rl::Layer
{
public:
    ExampleSandboxLayer();

    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event& event) override;

private:
    bool onKeyPressed(rl::KeyPressedEvent &event);

    rl::Ref<rl::VertexArray> m_smallSquare;
    rl::Ref<rl::Shader> m_testShader;
    //
    rl::Ref<rl::VertexArray> m_texturedSquare;
    rl::Ref<rl::Shader> m_texturedShader;
    rl::Ref<rl::Texture2D> m_squareTexture;
    rl::ShaderLibrary m_shaders;

    rl::OrthographicCameraController m_cameraController;

    //
    glm::vec4 clearColor{0.f};
    bool m_showDebugImGuiWindow = true;
};
