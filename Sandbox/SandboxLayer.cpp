#include "SandboxLayer.h"

#include <Rish/Input/Input.h>
#include <Rish/Utils/Math.h>

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
    std::shared_ptr<rl::VertexBuffer> vb;
    vb = std::make_shared<rl::VertexBuffer>(vertices, sizeof(vertices));
    rl::BufferLayout layout = {
        {rl::ShaderDataType::Float3, "a_Position"},
        {rl::ShaderDataType::Float4, "a_Color"}
    };
    vb->setLayout(layout);
    m_testVA->setVertexBuffer(vb);

    // Index Buffer
    uint32_t indices[3] = {0, 1, 2};
    std::shared_ptr<rl::IndexBuffer> ib;
    ib = std::make_shared<rl::IndexBuffer>(indices, 3);
    m_testVA->setIndexBuffer(ib);

    // Load shader
    std::string vertPath, fragPath;
    rl::VFS::ResolvePhysicalPath("/shader/test.vert", vertPath);
    rl::VFS::ResolvePhysicalPath("/shader/test.frag", fragPath);
    m_testShader = std::make_shared<rl::Shader>(vertPath.c_str(), fragPath.c_str());
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



    m_camera.setPosition(m_cameraPosition);
    m_camera.setRotation(m_cameraRotation);

    rl::Renderer::BeginScene(m_camera);
    rl::Renderer::Submit(m_testShader, m_testVA);
    rl::Renderer::EndScene();
}

void ExampleSandboxLayer::onImGuiRender()
{
    if(m_showDebugImGuiWindow)
    {
        defaultLogWindow.onImGuiRender();
    }

    ImGui::Begin("Debug");
    ImGui::Text("Camera");
    auto pos = m_camera.getPosition();
    ImGui::Text("Position = %.2f %.2f %.2f", pos.x, pos.y, pos.z);
    auto rot = m_camera.getRotation();
    ImGui::Text("Rotation = %.2f", rot);

    ImGui::Separator();
    ImGui::End();
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    if(event.getEventType() == rl::EventType::KeyPressed)
    {
        auto &keyEvent = dynamic_cast<rl::KeyPressedEvent&>(event);
        if(keyEvent.keyCode == rl::Keyboard::Tilde)
        {
            m_showDebugImGuiWindow = !m_showDebugImGuiWindow;
        }
    }
}
