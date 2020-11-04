#include <LightLayer.h>


LightLayer::LightLayer()
    : Layer("LightLayer"), m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    RL_TRACE("Light Layer Construct");
    radius = 100;
    strength = 100;
    color = glm::vec4(1, 1, 1, 1);
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
    glm::vec3 lightPos = {0, 0, 0};
    glm::vec2 viewPortSize = {Application::Get().getWindow().getWidth() / 2, Application::Get().getWindow().getHeight()/2};
    Renderer2D::BeginScene(m_cameraController.getCamera(), true);
    {
        Renderer2D::DrawPointLight(lightPos, color, radius, strength, lightPos, {1, 1});
        Renderer2D::DrawQuad({1, 1, 0}, {1, 1},{1, 0, 0, 1});
    }
    Renderer2D::EndScene();
}

void LightLayer::onImGuiRender()
{

    ImGui::Begin("Attribute");
    ImGui::ColorEdit4("Color", glm::value_ptr(color), ImGuiColorEditFlags_Float);
    ImGui::DragFloat("Radius", &radius, 1, 0, FLT_MAX, "%.2f");
    ImGui::DragFloat("Strength", &strength, 1, 0, FLT_MAX, "%.2f");
    ImGui::End();
}

void LightLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}