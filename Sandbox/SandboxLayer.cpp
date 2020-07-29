#include "SandboxLayer.h"

#include <Rish/Input/Input.h>
#include <Rish/Renderer/Texture2D.h>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("example"),
      m_cameraController((float)rl::Application::Get().getWindow().getWidth() / rl::Application::Get().getWindow().getHeight(), true)
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");
    std::string vertPath, fragPath, texPath;

    // Textured Square
    {
        m_texturedSquare = rl::VertexArray::Create();
        // Vertex Buffer
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
        };
        rl::Ref<rl::VertexBuffer> vb = rl::VertexBuffer::Create(vertices, sizeof(vertices));
        rl::BufferLayout layout = {
            {rl::ShaderDataType::Float3, "a_Position"},
            {rl::ShaderDataType::Float2, "a_TexCord"}
        };
        vb->setLayout(layout);
        m_texturedSquare->setVertexBuffer(vb);

        // Index Buffer
        uint32_t indices[] = {0, 1, 2, 2, 3, 0};
        rl::Ref<rl::IndexBuffer> ib = rl::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_texturedSquare->setIndexBuffer(ib);

        // Load texture
        //
        m_squareTexture = rl::Texture2D::LoadTextureVFS("/texture/1.png", true);
//        auto m_testImage = rl::Image::LoadImageVFS("/texture/1.png");
//        m_squareTexture = std::make_shared<rl::Texture2D>(m_testImage);

        // Load Shader
//        m_texturedShader = rl::Shader::LoadShaderVFS("/shader/textured.vert", "/shader/textured.frag");
        m_texturedShader = m_shaders.load("textured", "/shader/textured");
        m_texturedShader->bind();
        // TODO: Shader class should
        m_texturedShader->setInt("u_Texture", 0);
    }

    // Small Square
    {
        m_smallSquare = rl::VertexArray::Create();
        float square[] = {
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        };

        // Vertex Buffer
        rl::Ref<rl::VertexBuffer> sq = rl::VertexBuffer::Create(square, sizeof(square));
        rl::BufferLayout layout = {
            {rl::ShaderDataType::Float3, "a_Position"},
            {rl::ShaderDataType::Float4, "a_Color"}
        };
        sq->setLayout(layout);
        m_smallSquare->setVertexBuffer(sq);

        // Index Buffer
        uint32_t a[] = {0, 1, 2, 1, 2, 3};
        rl::Ref<rl::IndexBuffer> ib = rl::IndexBuffer::Create(a, 6);
        m_smallSquare->setIndexBuffer(ib);

        // Load shaders
//        m_testShader = rl::Shader::LoadShaderVFS("/shader/test.vert", "/shader/test.frag");
        m_testShader = m_shaders.load("test", "/shader/test");
    }
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    // Update
    m_cameraController.onUpdate(dt);

    // Render
    rl::RenderCommand::SetClearColor(clearColor);
    rl::RenderCommand::Clear();

    rl::Renderer::BeginScene(m_cameraController.getCamera());

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
        ImGui::Separator();
        ImGui::ColorEdit4("BG", glm::value_ptr(clearColor), ImGuiColorEditFlags_Float);
        ImGui::End();
        m_cameraController.onImGuiRender();
    }
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    rl::EventDispatcher dispatcher(event);
    dispatcher.dispatch<rl::KeyPressedEvent>(RL_BIND_EVENT_FUNC(ExampleSandboxLayer::onKeyPressed));
    m_cameraController.onEvent(event);
}

bool ExampleSandboxLayer::onKeyPressed(rl::KeyPressedEvent &event)
{
    if(event.keyCode == rl::Keyboard::Tilde)
    {
        m_showDebugImGuiWindow = !m_showDebugImGuiWindow;
    }
    return false;
}
