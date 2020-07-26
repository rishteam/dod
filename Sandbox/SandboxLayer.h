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
    std::shared_ptr<rl::VertexArray> m_testVA;
    std::shared_ptr<rl::Shader> m_testShader;
    rl::OrthographicCamera m_camera;
    bool m_showDebugImGuiWindow = false;
};
