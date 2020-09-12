#include "ParticleLayer.h"

#include <glm/gtc/type_ptr.hpp>

ParticleLayer::ParticleLayer()
    : Layer("ParticleLayer"),
      m_cameraController((float)rl::Application::Get().getWindow().getWidth() / (float)rl::Application::Get().getWindow().getHeight(), true),
      m_particleSystem(1000)
{
    RL_PROFILE_FUNCTION();

    RL_INFO("Current = {}", rl::FileSystem::GetCurrentDirectory());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    ImGui::LoadIniSettingsFromDisk("Sandbox/imgui.ini");

    m_particle.colorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
    m_particle.colorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
    m_particle.sizeBegin = 0.5f, m_particle.sizeVar = 0.3f, m_particle.sizeEnd = 0.0f;
    m_particle.lifeTime = 1.0f;
    m_particle.velocity = { 0.0f, 0.0f };
    m_particle.velocityVar = { 3.0f, 1.0f };
    m_particle.position = { 0.0f, 0.0f };
}

void ParticleLayer::onAttach()
{
    RL_PROFILE_FUNCTION();

    m_texture = rl::Texture2D::LoadTextureVFS("/texture/1.png");
    m_spriteSheet = rl::Texture2D::LoadTextureVFS("/texture/rs.png");
    RL_INFO("load sprite sheet {}x{}", m_spriteSheet->getWidth(), m_spriteSheet->getHeight());
    m_sprite = rl::SubTexture2D::CreateFromSheet(m_spriteSheet, {0, 0}, {17, 17}, {2, 2});
}

void ParticleLayer::onDetach()
{
    RL_PROFILE_FUNCTION();
}

void ParticleLayer::onUpdate(rl::Time dt)
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
        rl::Renderer2D::BeginScene(m_cameraController.getCamera(), nullptr);

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
        rl::Renderer2D::DrawQuad({-10.f, 2.f, 0.f}, {1.f, 1.f}, m_texture);

        rl::Renderer2D::DrawQuad({-10.0, 0.f}, {1.f, 1.f}, m_spriteSheet);
        rl::Renderer2D::DrawQuad({-10.0, -3.f}, {1.f, 1.f}, m_sprite, glm::vec4{1.f});

        rl::Renderer2D::DrawRotatedQuad({-9.0f, 0.f}, {1.f, 1.f}, m_sprite, glm::vec4{1.f}, m_objectRotate);

        rl::Renderer2D::DrawLine({-15.f, -15.f, 5.f}, {1.f, 1.f, 5.f});
        rl::Renderer2D::DrawLine({-15.f, -15.f, 5.f}, {1.f, 1.f, 5.f}, {1.f, 0.f, 0.f, 1.f});

        rl::Renderer2D::DrawQuad({-3, 1}, {1.f, 1.f}, {1.f, 1.f, 1.f, 1.f});
        rl::Renderer2D::DrawQuad({-3, 1}, {1.f, 1.f}, {1.f, 0.f, 0.f, 1.f});

        rl::Renderer2D::EndScene();
    }

    if (rl::Input::IsMouseButtonPressed(rl::Mouse::Left))
    {
        auto [x, y] = rl::Input::GetMousePosition();
        auto winW = rl::Application::Get().getWindow().getWidth();
        auto winH = rl::Application::Get().getWindow().getHeight();

        auto camBounds = m_cameraController.getBounds();
        auto camPos = m_cameraController.getCamera().getPosition();
        //
        x = (x / winW) * camBounds.getWidth() - camBounds.getWidth() * 0.5f;
        y = camBounds.getHeight() * 0.5f - (y / winH) * camBounds.getHeight();
        //
        m_particle.position = {x + camPos.x, y + camPos.y };

        //
        RL_INFO("Add particle ({}, {})", m_particle.position.x, m_particle.position.y);
        for (int i = 0; i < 5; i++)
            m_particleSystem.emit(m_particle);
    }

    m_particleSystem.onUpdate(dt);
    m_particleSystem.onRender(m_cameraController.getCamera());
}

void ParticleLayer::onImGuiRender()
{
    RL_PROFILE_FUNCTION();

    ImGui::Begin("info");
    {
        auto [x, y] = rl::Input::GetMousePosition();
        auto winW = rl::Application::Get().getWindow().getWidth();
        auto winH = rl::Application::Get().getWindow().getHeight();
        auto camBounds = m_cameraController.getBounds();
        auto camPos = m_cameraController.getCamera().getPosition();
        x = (x / winW) * camBounds.getWidth() - camBounds.getWidth() * 0.5f;
        y = camBounds.getHeight() * 0.5f - (y / winH) * camBounds.getHeight();
        ImGui::Begin("info");
        ImGui::Text("tx = %.2f ty = %.2f", x, y);
        ImGui::End();

        ImGui::Text("Mouse Pos  = %.2f %.2f", x, y);
        ImGui::Text("Windows    = %d %d", winW, winH);
        ImGui::Text("cam pos    = %.2f %.2f %.2f", camPos.x, camPos.y, camPos.z);
        ImGui::Text("cam bounds = %.2f %.2f %.2f %.2f", camBounds.left, camBounds.right, camBounds.top, camBounds.bottom);
    }
    ImGui::End();

    ImGui::Begin("Debug");
        ImGui::Text("FPS = %d", rl::Application::Get().getFps());
        ImGui::Text("Draw = %d Quad count = %d", rl::Renderer2D::GetStats().DrawCount, rl::Renderer2D::GetStats().QuadCount);
        ImGui::DragInt("W", &m_gridWidth);
        ImGui::DragInt("H", &m_gridHeight);
        ImGui::Separator();
        if(ImGui::TreeNode("Attribute"))
        {
            ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor), ImGuiColorEditFlags_Float);
            ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor), ImGuiColorEditFlags_Float);
            ImGui::Separator();
            ImGui::Text("Square");
            ImGui::DragFloat3("Position", glm::value_ptr(m_squarePosition), 0.1f);
            ImGui::DragFloat2("Scale", glm::value_ptr(m_squareScale), 0.1f);
            ImGui::DragFloat("Rotate", &m_objectRotate, 0.1f);
            ImGui::TreePop();
        }
    ImGui::End();
    m_cameraController.onImGuiRender();
}

void ParticleLayer::onEvent(rl::Event &e)
{
    RL_PROFILE_FUNCTION();
    m_cameraController.onEvent(e);
}
