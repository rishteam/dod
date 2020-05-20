#pragma once

#include "Rish/Core/Core.h"

namespace rl {

class RL_API Application
{
public:
    Application();
    virtual ~Application();

    void run();
};

Application* CreateApplication();

}