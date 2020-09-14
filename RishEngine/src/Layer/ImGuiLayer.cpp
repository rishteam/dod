#include <Rish/ImGui.h>

#include <SFML/Window/Keyboard.hpp>
#include <Rish/Core/Application.h>

#include <SFML/OpenGL.hpp>
#include <Rish/Layer/ImGuiLayer.h>

namespace rl {

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer"),
    m_window(Application::Get().getWindow())
{
    RL_PROFILE_FUNCTION();

}

ImGuiLayer::~ImGuiLayer()
{
    RL_PROFILE_FUNCTION();

}

void ImGuiLayer::onAttach()
{
    RL_PROFILE_FUNCTION();

    RL_CORE_TRACE("[{}] onAttach", GetName());
    Application::Get().getWindow().initImGui();
}

void ImGuiLayer::onDetach()
{
    RL_PROFILE_FUNCTION();

    RL_CORE_TRACE("[{}] onDetach", GetName());
    Application::Get().getWindow().shutdownImGui();
}

void ImGuiLayer::begin()
{
    RL_PROFILE_FUNCTION();

    RL_CORE_ASSERT(ImGui::GetCurrentContext(), "ImGui has no context");
    //
    ImGuiIO &io = ImGui::GetIO();
    auto &window = Application::Get().getWindow();
    io.DisplaySize = ImVec2((float)window.getWidth(), (float)window.getHeight());

    Application::Get().getWindow().updateImGui();
}

void ImGuiLayer::end()
{
    RL_PROFILE_FUNCTION();

    RL_CORE_ASSERT(ImGui::GetCurrentContext(), "ImGui has no context");
    //
    Application::Get().getWindow().renderImGui();
}

void ImGuiLayer::onEvent(Event &event)
{
    RL_PROFILE_FUNCTION();

    RL_CORE_ASSERT(ImGui::GetCurrentContext(), "ImGui has no context");
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
    io.MousePos = ImVec2((float)event.x, (float)event.y);
    return false;
}

bool ImGuiLayer::onMouseScrolled(MouseScrolledEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += event.xOffset;
    io.MouseWheel += event.yOffset;
    return false;
}

bool ImGuiLayer::onKeyPressed(KeyPressedEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    io.KeysDown[event.keyCode] = true; // set the key
    // Special Keys
    io.KeyCtrl = io.KeysDown[sf::Keyboard::LControl] || io.KeysDown[sf::Keyboard::RControl];
    io.KeyShift = io.KeysDown[sf::Keyboard::LShift] || io.KeysDown[sf::Keyboard::RShift];
    io.KeyAlt = io.KeysDown[sf::Keyboard::LAlt] || io.KeysDown[sf::Keyboard::RAlt];
    io.KeySuper = io.KeysDown[sf::Keyboard::LSystem] || io.KeysDown[sf::Keyboard::RSystem];
    return false;
}

bool ImGuiLayer::onKeyReleased(KeyReleasedEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    io.KeysDown[event.keyCode] = false; // unset the key
    return false;
}

bool ImGuiLayer::onKeyTyped(KeyTypedEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    int keycode = event.keyCode;
    if (keycode > 0 && keycode < 0x10000)
        io.AddInputCharacter((unsigned int)keycode);
    return false;
}

bool ImGuiLayer::onWindowResize(WindowResizeEvent &event)
{
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(event.m_width, event.m_height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    glViewport(0, 0, event.m_width, event.m_height);
    return false;
}

}