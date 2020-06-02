#include <Rish/Core/Application.h>
#include <Rish/Layer/ImGuiLayer.h>

namespace rl {

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
}

ImGuiLayer::~ImGuiLayer()
{
}

void ImGuiLayer::onAttach()
{
    Application::Get().getWindow().initImGui();
}

void ImGuiLayer::onDetach()
{
    Application::Get().getWindow().shutdownImGui();
}

void ImGuiLayer::onUpdate()
{
    Application::Get().getWindow().updateImGui();

    ImGui::Begin("123");
    ImGui::End();

    Application::Get().getWindow().renderImGui();
}

void ImGuiLayer::onEvent(Event &event)
{

}

}
