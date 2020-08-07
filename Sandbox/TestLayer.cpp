#include "TestLayer.h"

#include <glm/gtc/type_ptr.hpp>

TestLayer::TestLayer()
    : Layer("TestLayer"),
      m_cameraController((float)rl::Application::Get().getWindow().getWidth() / (float)rl::Application::Get().getWindow().getHeight(), true),
      m_particleSystem(10000)
{
    RL_PROFILE_FUNCTION();

    RL_INFO("Current = {}", rl::FileSystem::GetCurrentDirectory());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    m_particle.colorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
    m_particle.colorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
    m_particle.sizeBegin = 0.5f, m_particle.sizeVar = 0.3f, m_particle.sizeEnd = 0.0f;
    m_particle.lifeTime = 1.0f;
    m_particle.velocity = { 0.0f, 0.0f };
    m_particle.velocityVar = { 3.0f, 1.0f };
    m_particle.position = { 0.0f, 0.0f };
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
                if((i+j) % 2)
                    rl::Renderer2D::DrawRotatedQuad({i, j, 0.0f}, {0.8f, 0.8f}, {i / 10.f, j / 10.f, 1.f, 1.f}, m_objectRotate);
                else
                    rl::Renderer2D::DrawQuad({i, j, 0.0f}, {0.8f, 0.8f}, {i / 10.f, j / 10.f, 1.f, 1.f});
            }
        rl::Renderer2D::DrawQuad({0.3f, 0.9f, 0.f}, {1.f, 1.f}, m_texture);

        rl::Renderer2D::EndScene();
    }

    // TODO: bug and figure out
    if (rl::Input::isMouseButtonPressed(rl::Mouse::Left))
    {
        auto [x, y] = rl::Input::getMousePosition();
        auto width = rl::Application::Get().getWindow().getWidth();
        auto height = rl::Application::Get().getWindow().getHeight();

        auto bounds = m_cameraController.getBounds();
        auto pos = m_cameraController.getCamera().getPosition();
        x = (x / width) * bounds.getWidth() - bounds.getWidth() * 0.5f;
        y = bounds.getHeight() * 0.5f - (y / height) * bounds.getHeight();
        m_particle.position = { x + pos.x, y + pos.y };
        for (int i = 0; i < 5; i++)
            m_particleSystem.emit(m_particle);
    }

    m_particleSystem.onUpdate(dt);
    m_particleSystem.onRender(m_cameraController.getCamera());
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
