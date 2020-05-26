#include <SFML/OpenGL.hpp>

#include "Rish/Core/Application.h"

#include "Rish/Events/Event.h"
#include "Rish/Events/ApplicationEvent.h"

#include <fmt/printf.h>
#include <fmt/format.h>

namespace rl {

#define BIND_APPEVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
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
        m_window->onUpdate();

        if(m_window)
            m_window->onDraw();
    }
}

void Application::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(BIND_APPEVENT_FUNC(onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(BIND_APPEVENT_FUNC(onWindowResize));
}

bool Application::onWindowClose(WindowCloseEvent &e)
{
    RL_CORE_TRACE("Close : {}", e);
    m_running = false;
    return true;
}

bool Application::onWindowResize(WindowResizeEvent &e)
{
    RL_CORE_TRACE("{}", e);
    glViewport(0, 0, e.m_width, e.m_height);
}

}