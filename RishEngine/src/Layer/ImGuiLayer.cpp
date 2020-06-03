#include <imgui.h>

#include <Rish/Core/Application.h>
#include <Rish/Layer/ImGuiLayer.h>

namespace rl {

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer"),
    m_window(Application::Get().getWindow())
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
    if (!ImGui::GetCurrentContext())
        return;

    ImGuiIO& io = ImGui::GetIO();
    auto &window = Application::Get().getWindow();
    io.DisplaySize = ImVec2(window.getWidth(), window.getHeight());

    RL_CORE_TRACE("{} {}", io.MousePos.x, io.MousePos.y);

    Application::Get().getWindow().updateImGui();

    ImGui::Begin("123");
    ImGui::End();

    Application::Get().getWindow().renderImGui();
}

void ImGuiLayer::onEvent(Event &event)
{
    if(!ImGui::GetCurrentContext())
        return;
    //
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<MouseButtonPressedEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onMouseButtonPressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onMouseButtonReleased));
    dispatcher.dispatch<MouseMovedEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onMouseMoved));
    dispatcher.dispatch<MouseScrolledEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onMouseScrolled));
    dispatcher.dispatch<KeyPressedEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onKeyPressed));
    dispatcher.dispatch<KeyReleasedEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onKeyReleased));
    dispatcher.dispatch<KeyTypedEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onKeyTyped));
    dispatcher.dispatch<WindowResizeEvent>(RL_BIND_EVENT_FUNC(ImGuiLayer::onWindowResize));
}

bool ImGuiLayer::onMouseButtonPressed(MouseButtonPressedEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[event.button] = true;
    return false;
}

bool ImGuiLayer::onMouseButtonReleased(MouseButtonReleasedEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[event.button] = false;
    return false;
}

bool ImGuiLayer::onMouseMoved(MouseMovedEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(event.x, event.y);
    return false;
}

bool ImGuiLayer::onMouseScrolled(MouseScrolledEvent &event)
{
}

bool ImGuiLayer::onKeyPressed(KeyPressedEvent &event)
{
}

bool ImGuiLayer::onKeyReleased(KeyReleasedEvent &event)
{
}

bool ImGuiLayer::onKeyTyped(KeyTypedEvent &event)
{
}

bool ImGuiLayer::onWindowResize(WindowResizeEvent &event)
{
}

}
