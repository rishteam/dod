#pragma once

#include "Rish/rlpch.h"

#include "Rish/Core/Core.h"
#include "Rish/Core/Window.h"
#include "Rish/Events/ApplicationEvent.h"

namespace rl {

class RL_API Application
{
public:
    Application();
    virtual ~Application();

    void run();
    void onEvent(Event &e);
private:
    bool onWindowClose(WindowCloseEvent &e);
    bool onWindowResize(WindowResizeEvent &e);

    std::unique_ptr<Window> m_window;
    bool m_running = false;
};

Application* CreateApplication();

}