#include "SandboxLayer.h"

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
    rl::Renderer::BeginScene();

    m_testShader->bind();
    m_testShader->setMat4("u_ViewProjection", m_camera.getViewProjectionMatrix());
    rl::Renderer::Submit(m_testVA);

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
    ImGui::DragFloat3("Pos", glm::value_ptr(pos), 0.01f);
    m_camera.setPosition(pos);

    auto rot = m_camera.getRotation();
    ImGui::DragFloat("Rotation", &rot, 1.f);
    m_camera.setRotation(rot);
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
