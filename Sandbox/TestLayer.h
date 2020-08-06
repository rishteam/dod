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

    glm::vec3 m_squarePosition{0.f, 0.f, 0.f};
    glm::vec2 m_squareScale{1.f, 1.f};
    rl::Ref<rl::Texture2D> m_texture;
    float m_objectRotate = 0.f;

    int m_gridWidth = 100, m_gridHeight = 100;

    glm::vec4 m_squareColor{1.f, 0.f, 0.f, 1.f};
    glm::vec4 m_clearColor{0.f, 0.f, 0.f, 1.f};
};