#include "Rish/Core/Application.h"

#include "Rish/Events/ApplicationEvent.h"

#include <fmt/printf.h>
#include <fmt/format.h>

namespace rl {

Application::Application()
{
}

Application::~Application()
{
}

void Application::run()
{
    AppRenderEvent e;

    RL_INFO("{}", e);

    while(1);
}

}