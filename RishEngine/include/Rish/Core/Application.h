#pragma once

#include "Rish/rlpch.h"

#include "Rish/Core/Core.h"
#include "Rish/Core/Window.h"
#include "Rish/Events/ApplicationEvent.h"
#include "Rish/Layer/LayerStack.h"

namespace rl {

/**
 * @brief Application class
 * @details The main application
 */
class RL_API Application
{
public:
    Application();
    virtual ~Application();

    void run();
    void onEvent(Event &e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);
    void popLayer(Layer* layer);
    void popOverlay(Layer* overlay);
private:
    bool onWindowClose(WindowCloseEvent &e);
    bool onWindowResize(WindowResizeEvent &e);

    std::unique_ptr<Window> m_window;
    bool m_running = false;
    LayerStack m_LayerStack;
};

Application* CreateApplication();

}