#include "SandboxLayer.h"

#include <Rish/Input/Input.h>
#include <cmath>

#include <glm/gtc/type_ptr.hpp>

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("example"),
      m_camera(-1.6f, 1.6f, -0.9f, 0.9f)
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");
    m_testVA = std::make_shared<rl::VertexArray>();

    // Vertex Buffer
    float vertices[3 * 7] = {
        0.0f,  0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
        -0.5f,  -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
        0.5f,  -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };
    rl::Ref<rl::VertexBuffer> vb;
    vb = std::make_shared<rl::VertexBuffer>(vertices, sizeof(vertices));
    rl::BufferLayout layout = {
        {rl::ShaderDataType::Float3, "a_Position"},
        {rl::ShaderDataType::Float4, "a_Color"}
    };
    vb->setLayout(layout);
    m_testVA->setVertexBuffer(vb);

    // Index Buffer
    uint32_t indices[3] = {0, 1, 2};
    rl::Ref<rl::IndexBuffer> ib;
    ib = std::make_shared<rl::IndexBuffer>(indices, 3);
    m_testVA->setIndexBuffer(ib);

    // Load shader
    std::string vertPath, fragPath;
    rl::VFS::ResolvePhysicalPath("/shader/test.vert", vertPath);
    rl::VFS::ResolvePhysicalPath("/shader/test.frag", fragPath);
    m_testShader = std::make_shared<rl::Shader>(vertPath.c_str(), fragPath.c_str());

    // Square
    m_testSquare = std::make_shared<rl::VertexArray>();
    float square[] = {
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    };
    rl::Ref<rl::VertexBuffer> sq = std::make_shared<rl::VertexBuffer>(square, sizeof(square));
    rl::BufferLayout sqlayout = {
        {rl::ShaderDataType::Float3, "a_Position"},
        {rl::ShaderDataType::Float4, "a_Color"}
    };
    sq->setLayout(sqlayout);
    m_testSquare->setVertexBuffer(sq);
    uint32_t a[] = {0, 1, 2, 1, 2, 3};
    ib = std::make_shared<rl::IndexBuffer>(a, 6);
    m_testSquare->setIndexBuffer(ib);

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
    rl::Renderer::Submit(m_testShader, m_testVA);

    glm::mat4 sacle = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));
    for(int j = 0; j < 20; j++)
        for(int i = 0; i < 20; i++)
        {
            glm::vec3 pos(i * 0.11f, j*0.11f, 0.0f);
            glm::mat4 trans = glm::translate(glm::mat4(1.f), pos) * sacle;
            rl::Renderer::Submit(m_testShader, m_testSquare, trans);
        }
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
