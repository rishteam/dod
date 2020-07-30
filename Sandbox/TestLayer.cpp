#include "TestLayer.h"

#include <glm/gtc/type_ptr.hpp>

TestLayer::TestLayer()
    : Layer("TestLayer"),
      m_cameraController((float)rl::Application::Get().getWindow().getWidth() / (float)rl::Application::Get().getWindow().getHeight(), true)
{
    RL_INFO("Current = {}", rl::FileSystem::GetCurrentDirectory());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

}

void TestLayer::onAttach()
{
}

void TestLayer::onDetach()
{
}

void TestLayer::onUpdate(rl::Time dt)
{
    // Update
    m_cameraController.onUpdate(dt);

    rl::RenderCommand::SetClearColor(m_clearColor);
    rl::RenderCommand::Clear();

    // Render
    rl::Renderer2D::BeginScene(m_cameraController.getCamera());
    rl::Renderer2D::DrawQuad({0.f, 0.f}, {1.f, 1.f}, m_squareColor);
    rl::Renderer2D::EndScene();

//    rl::Renderer::BeginScene(m_cameraController.getCamera());
//    rl::Renderer::Submit(m_testShader, m_smallSquare);
//    rl::Renderer::EndScene();
}

void TestLayer::onImGuiRender()
{
    m_cameraController.onImGuiRender();

    ImGui::Begin("Debug");
    ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor), ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor), ImGuiColorEditFlags_Float);
    ImGui::End();
}

void TestLayer::onEvent(rl::Event &e)
{
    m_cameraController.onEvent(e);
}
