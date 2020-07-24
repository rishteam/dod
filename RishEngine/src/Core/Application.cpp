#include "Rish/Core/Core.h"
#include "Rish/Core/Application.h"
#include "Rish/Core/Time.h"

#include "Rish/Events/Event.h"
#include "Rish/Events/ApplicationEvent.h"

#include <fmt/printf.h>
#include <fmt/format.h>

#include <SFML/System/Clock.hpp>
#include <SFML/OpenGL.hpp>
#include <imgui.h>

// #include "Rish/Renderer/Buffer.h"
// #include "Rish/Renderer/VertexArray.h"

namespace rl {

#define S_PER_UPDATE 0.01

Application *Application::s_instance = nullptr;

Application::Application(const std::string &name, uint32_t width, uint32_t height)
{
    RL_CORE_ASSERT(s_instance == nullptr, "RishEngine should only have ONE Application instance");
    Application::s_instance = this; // set instance
    m_window = std::unique_ptr<Window>(Window::Create(name, width, height));
    m_window->setEventCallback(RL_BIND_EVENT_FUNC(Application::onEvent));
    m_running = true; // set the running flag
    // Push the imgui overlay
    m_imguiLayer = new ImGuiLayer();
    pushOverlay(m_imguiLayer);
}

Application::~Application()
{
}

void Application::run()
{
    // static sf::Clock clk; // TODO(roy4801): make RishEngine clock in the future
    Clock clk;
    float lag = 0.0;

    while(m_running)
    {
        // Update time
        float now = clk.getElapsedTime().asSeconds();
        Time dt = now - m_prevFrameTime;
        m_prevFrameTime = now;
        lag += dt;

        // Update window
        m_window->onUpdate();

        // Update Timer
        Timer::Update();

        // Update layers
        for(Layer* layer: m_LayerStack)
            layer->onUpdate(dt);

        // Update ImGui
        m_imguiLayer->begin();
        for(Layer* layer : m_LayerStack)
            layer->onImGuiRender();
        m_imguiLayer->end();

        // Draw window
        if(m_window)
            m_window->onDraw();
    }
}

void Application::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(RL_BIND_EVENT_FUNC(Application::onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(RL_BIND_EVENT_FUNC(Application::onWindowResize));

    for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
    {
        (*it)->onEvent(e);
        // if event is handled then break
        if(e.handled)
            break;
    }
}

void Application::pushLayer(Layer* layer)
{
    m_LayerStack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay)
{
    m_LayerStack.pushOverlay(overlay);
}

void Application::popLayer(Layer* layer)
{
    m_LayerStack.popLayer(layer);
}

void Application::popOverlay(Layer* overlay)
{
    m_LayerStack.popOverlay(overlay);
}

bool Application::onWindowClose(WindowCloseEvent &e)
{
    RL_CORE_TRACE("{}", e);
    m_running = false;
    return true;
}

bool Application::onWindowResize(WindowResizeEvent &e)
{
    RL_CORE_TRACE("{}", e);
    glViewport(0, 0, e.m_width, e.m_height);
    return false;
}

}
