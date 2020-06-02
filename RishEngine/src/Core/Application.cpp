#include <SFML/OpenGL.hpp>

#include "Rish/Core/Application.h"

#include "Rish/Events/Event.h"
#include "Rish/Events/ApplicationEvent.h"

#include <fmt/printf.h>
#include <fmt/format.h>

namespace rl {

Application *Application::s_instance = nullptr;

#define BIND_APPEVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
    RL_CORE_ASSERT(s_instance == nullptr, "RishEngine should only have ONE Application instance");
    Application::s_instance = this; // set instance
    m_window = std::unique_ptr<Window>(Window::Create());
    m_window->setEventCallback(BIND_APPEVENT_FUNC(onEvent));

    m_running = true;
}

Application::~Application()
{
}

void Application::run()
{
    while(m_running)
    {
        // Update window
        m_window->onUpdate();
        // Update layers
        for(Layer* layer: m_LayerStack)
            layer->onUpdate();
        // Draw window
        if(m_window)
            m_window->onDraw();
    }
}

void Application::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(BIND_APPEVENT_FUNC(onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(BIND_APPEVENT_FUNC(onWindowResize));

    // RL_CORE_INFO("{}", e.toString());

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
}

}
