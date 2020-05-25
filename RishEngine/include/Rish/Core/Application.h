#pragma once

#include "Rish/rlpch.h"

#include "Rish/Core/Core.h"
#include "Rish/Core/Window.h"

namespace rl {

class RL_API Application
{
public:
    Application();
    virtual ~Application();

    void run();
private:
    std::unique_ptr<Window> m_window;
};

Application* CreateApplication();

}