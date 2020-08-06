#include "TestLayer.h"

#include <glm/gtc/type_ptr.hpp>

TestLayer::TestLayer()
    : Layer("TestLayer"),
      m_cameraController((float)rl::Application::Get().getWindow().getWidth() / (float)rl::Application::Get().getWindow().getHeight(), true)
{
    RL_PROFILE_FUNCTION();

    RL_INFO("Current = {}", rl::FileSystem::GetCurrentDirectory());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");
}

void TestLayer::onAttach()
{
    RL_PROFILE_FUNCTION();

    m_texture = rl::Texture2D::LoadTextureVFS("/texture/1.png");
}

void TestLayer::onDetach()
{
    RL_PROFILE_FUNCTION();
}

void TestLayer::onUpdate(rl::Time dt)
{
    RL_PROFILE_FUNCTION();
    // Update
    m_cameraController.onUpdate(dt);

    {
        RL_PROFILE_SCOPE("Renderer Prepare");
        rl::RenderCommand::SetClearColor(m_clearColor);
        rl::RenderCommand::Clear();
    }
    static float rotateSpeed = 180;
    m_objectRotate += rotateSpeed * dt.asSeconds();
    if(m_objectRotate >= 360.f || m_objectRotate <= -360.f)
        m_objectRotate = std::fmod(m_objectRotate, 360.f);

    // Render
    {
        RL_PROFILE_SCOPE("Renderer Draw");
        rl::Renderer2D::ResetStats();
        rl::Renderer2D::BeginScene(m_cameraController.getCamera());

        rl::Renderer2D::DrawQuad({-0.9f, 0.5f, -0.5f}, {0.5f, 0.5f}, {0.f, 1.f, 0.f, 1.f});
        rl::Renderer2D::DrawQuad({0.3f, -0.5f, -0.5f}, {0.5f, 0.5f}, {0.f, 0.f, 1.f, 1.f});
        rl::Renderer2D::DrawRotatedQuad({0.8f, 0.6f, 0.f}, {1.f, 1.f}, m_texture, m_objectRotate);
        rl::Renderer2D::DrawQuad(m_squarePosition, m_squareScale, m_squareColor);
        for(int i = 0; i < m_gridWidth; i++)
            for(int j = 0; j < m_gridHeight; j++)
            {
                rl::Renderer2D::DrawQuad({i, j, 0.0f}, {0.8f, 0.8f}, {i / 10.f, j / 10.f, 1.f, 1.f});
            }
        rl::Renderer2D::DrawQuad({0.3f, 0.9f, 0.f}, {1.f, 1.f}, m_texture);

        rl::Renderer2D::EndScene();
    }
}

void TestLayer::onImGuiRender()
{
    RL_PROFILE_FUNCTION();

    ImGui::Begin("Debug");
        ImGui::Text("FPS = %d", rl::Application::Get().getFps());
        ImGui::Text("Draw = %d Quad count = %d", rl::Renderer2D::GetStats().DrawCount, rl::Renderer2D::GetStats().QuadCount);
        ImGui::DragInt("W", &m_gridWidth);
        ImGui::DragInt("H", &m_gridHeight);
        ImGui::Separator();
        ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor), ImGuiColorEditFlags_Float);
        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor), ImGuiColorEditFlags_Float);
        ImGui::Separator();
        ImGui::Text("Square");
        ImGui::DragFloat3("Position", glm::value_ptr(m_squarePosition), 0.1f);
        ImGui::DragFloat2("Scale", glm::value_ptr(m_squareScale), 0.1f);
        ImGui::DragFloat("Rotate", &m_objectRotate, 0.1f);
    ImGui::End();
    m_cameraController.onImGuiRender();
}

void TestLayer::onEvent(rl::Event &e)
{
    RL_PROFILE_FUNCTION();
    m_cameraController.onEvent(e);
}
