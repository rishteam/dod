#include <LightLayer.h>


LightLayer::LightLayer()
    : Layer("LightLayer"), m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    RL_TRACE("Light Layer Construct");
    radius = 100;
    strength = 100;
    color = glm::vec4(1, 1, 1, 1);
    lightPos = glm::vec3(0, 0, 0);
}

LightLayer::~LightLayer()
{

}

void LightLayer::onAttach()
{
}

void LightLayer::onDetach()
{

}

void LightLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);
    glm::vec2 viewPortSize = {Application::Get().getWindow().getWidth() / 2, Application::Get().getWindow().getHeight()/2};
    Renderer2D::BeginScene(m_cameraController.getCamera(), false);
    {
        Renderer2D::DrawPointLight(lightPos, radius, strength, lightPos,
                                   viewPortSize, {Application::Get().getWindow().getWidth(), Application::Get().getWindow().getHeight()}, {m_cameraController.getBounds().right, m_cameraController.getBounds().top}, color);
//        Renderer2D::DrawQuad({0, 0}, {2, 2}, {1, 1, 1, 1});
    }
    Renderer2D::EndScene();
}

void LightLayer::onImGuiRender()
{

    ImGui::Begin("Attribute");
    ImGui::ColorEdit4("Color", glm::value_ptr(color), ImGuiColorEditFlags_Float);
    ImGui::DragFloat("Radius", &radius, 1, 0, FLT_MAX, "%.2f");
    ImGui::DragFloat("Strength", &strength, 1, 0, FLT_MAX, "%.2f");
    ImGui::DragFloat("PosX", &lightPos.x, 0.01);
    ImGui::DragFloat("PosY", &lightPos.y, 0.01);
    ImGui::End();
}

void LightLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
