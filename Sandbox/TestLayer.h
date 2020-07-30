#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

class TestLayer : public rl::Layer
{
public:
    TestLayer();

    void onAttach() override;
    void onDetach() override;

    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event &e) override;
private:
    rl::OrthographicCameraController m_cameraController;

    glm::vec4 m_squareColor{1.f, 0.f, 0.f, 1.f};
    glm::vec4 m_clearColor{0.f, 0.f, 0.f, 1.f};
};