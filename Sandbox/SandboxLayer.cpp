#include "SandboxLayer.h"

#include <Rish/Input/Input.h>
#include <Rish/Renderer/Texture2D.h>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("example"),
      m_camera(-1.6f, 1.6f, -0.9f, 0.9f)
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");
    std::string vertPath, fragPath, texPath;

    // Textured Square
    {
        m_texturedSquare = std::make_shared<rl::VertexArray>();
        // Vertex Buffer
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
        };
        rl::Ref<rl::VertexBuffer> vb;
        vb = std::make_shared<rl::VertexBuffer>(vertices, sizeof(vertices));
        rl::BufferLayout layout = {
            {rl::ShaderDataType::Float3, "a_Position"},
            {rl::ShaderDataType::Float2, "a_TexCord"}
        };
        vb->setLayout(layout);
        m_texturedSquare->setVertexBuffer(vb);

        // Index Buffer
        uint32_t indices[] = {0, 1, 2, 2, 3, 0};
        rl::Ref<rl::IndexBuffer> ib;
        ib = std::make_shared<rl::IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
        m_texturedSquare->setIndexBuffer(ib);

        // Load texture
        auto m_testImage = rl::Image::LoadImageVFS("/texture/1.png");
        m_squareTexture = rl::Texture2D::LoadTextureVFS("/texture/1.png");
        m_squareTexture = std::make_shared<rl::Texture2D>(m_testImage);

        // Load Shader
        m_texturedShader = rl::Shader::LoadShaderVFS("/shader/textured.vert", "/shader/textured.frag");
        m_texturedShader->bind();
        // TODO: Shader class should
        m_texturedShader->setInt("u_Texture", 0);
    }

    // Small Square
    {
        m_smallSquare = std::make_shared<rl::VertexArray>();
        float square[] = {
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        };

        // Vertex Buffer
        rl::Ref<rl::VertexBuffer> sq = std::make_shared<rl::VertexBuffer>(square, sizeof(square));
        rl::BufferLayout layout = {
            {rl::ShaderDataType::Float3, "a_Position"},
            {rl::ShaderDataType::Float4, "a_Color"}
        };
        sq->setLayout(layout);
        m_smallSquare->setVertexBuffer(sq);

        // Index Buffer
        uint32_t a[] = {0, 1, 2, 1, 2, 3};
        rl::Ref<rl::IndexBuffer> ib = std::make_shared<rl::IndexBuffer>(a, 6);
        m_smallSquare->setIndexBuffer(ib);

        // Load shaders
        m_testShader = rl::Shader::LoadShaderVFS("/shader/test.vert", "/shader/test.frag");
    }
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    if(rl::Input::isKeyPressed(rl::Keyboard::W))
    {
        m_cameraPosition.y += m_cameraMoveSpeed * dt.asSeconds();
    }
    else if(rl::Input::isKeyPressed(rl::Keyboard::S))
    {
        m_cameraPosition.y -= m_cameraMoveSpeed * dt.asSeconds();
    }
    if(rl::Input::isKeyPressed(rl::Keyboard::A))
    {
        m_cameraPosition.x -= m_cameraMoveSpeed * dt.asSeconds();
    }
    else if(rl::Input::isKeyPressed(rl::Keyboard::D))
    {
        m_cameraPosition.x += m_cameraMoveSpeed * dt.asSeconds();
    }
    if(rl::Input::isKeyPressed(rl::Keyboard::Q))
    {
        m_cameraRotation -= m_cameraRotateSpeed * dt.asSeconds();
    }
    else if(rl::Input::isKeyPressed(rl::Keyboard::E))
    {
        m_cameraRotation += m_cameraRotateSpeed * dt.asSeconds();
    }

    if(m_cameraRotation > 360.f || m_cameraRotation < -360.f)
        m_cameraRotation = std::fmod(m_cameraRotation, 360.f);

    m_camera.setPosition(m_cameraPosition);
    m_camera.setRotation(m_cameraRotation);

    rl::RenderCommand::SetClearColor(clearColor);
    rl::RenderCommand::Clear();

    rl::Renderer::BeginScene(m_camera);

    glm::mat4 sacle = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));
    for(int j = 0; j < 20; j++)
        for(int i = 0; i < 20; i++)
        {
            glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
            glm::mat4 trans = glm::translate(glm::mat4(1.f), pos) * sacle;
            rl::Renderer::Submit(m_testShader, m_smallSquare, trans);
        }

    m_squareTexture->bind();
    rl::Renderer::Submit(m_texturedShader, m_texturedSquare);
    m_squareTexture->unbind();

    rl::Renderer::EndScene();
}

void ExampleSandboxLayer::onImGuiRender()
{
    if(m_showDebugImGuiWindow)
    {
        defaultLogWindow.onImGuiRender();
        ImGui::Begin("Debug");
        ImGui::Text("Camera");
        auto pos = m_camera.getPosition();
        ImGui::Text("Position = %.2f %.2f %.2f", pos.x, pos.y, pos.z);
        auto rot = m_camera.getRotation();
        ImGui::Text("Rotation = %.2f", rot);

        ImGui::Separator();
        ImGui::ColorEdit4("BG", glm::value_ptr(clearColor), ImGuiColorEditFlags_Float);
        ImGui::End();
    }
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    rl::EventDispatcher dispatcher(event);
    dispatcher.dispatch<rl::KeyPressedEvent>(RL_BIND_EVENT_FUNC(ExampleSandboxLayer::onKeyPressed));
}

bool ExampleSandboxLayer::onKeyPressed(rl::KeyPressedEvent &event)
{
    if(event.keyCode == rl::Keyboard::Tilde)
    {
        m_showDebugImGuiWindow = !m_showDebugImGuiWindow;
    }
    return false;
}
