#include "Rish/Core/Application.h"

#include "Rish/Events/ApplicationEvent.h"

#include <fmt/printf.h>
#include <fmt/format.h>

namespace rl {

Application::Application()
{
    m_window = std::unique_ptr<Window>(Window::Create());
}

Application::~Application()
{
}

void Application::run()
{
    while(m_window->isOpen())
    {
        m_window->onUpdate();
        m_window->onDraw();
    }
}

}