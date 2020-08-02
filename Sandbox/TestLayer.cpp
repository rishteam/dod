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
}

void TestLayer::onUpdate(rl::Time dt)
{
    RL_PROFILE_FUNCTION();
    // Update
    m_cameraController.onUpdate(dt);

    rl::RenderCommand::SetClearColor(m_clearColor);
    rl::RenderCommand::Clear();

    // Render
    rl::Renderer2D::BeginScene(m_cameraController.getCamera());
    rl::Renderer2D::DrawQuad({-0.9f, 0.5f, -0.5f}, {0.5f, 0.5f}, {0.f, 1.f, 0.f, 1.f});
    rl::Renderer2D::DrawQuad({0.3f, -0.5f, -0.5f}, {0.5f, 0.5f}, {0.f, 0.f, 1.f, 1.f});
    rl::Renderer2D::DrawQuad({0.8f, 0.6f, 0.f}, {1.f, 1.f}, m_texture);
    rl::Renderer2D::DrawQuad(m_squarePosition, m_squareScale, m_squareColor);
    rl::Renderer2D::EndScene();
}

void TestLayer::onImGuiRender()
{
    RL_PROFILE_FUNCTION();
    m_cameraController.onImGuiRender();

    ImGui::Begin("Debug");
    ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor), ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor), ImGuiColorEditFlags_Float);
    ImGui::Separator();
    ImGui::Text("Square");
    ImGui::DragFloat3("Position", glm::value_ptr(m_squarePosition), 0.1f);
    ImGui::DragFloat2("Scale", glm::value_ptr(m_squareScale), 0.1f);
    ImGui::End();
}

void TestLayer::onEvent(rl::Event &e)
{
    RL_PROFILE_FUNCTION();
    m_cameraController.onEvent(e);
}
