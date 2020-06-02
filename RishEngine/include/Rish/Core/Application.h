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
    static Application& Get() { return *s_instance; }
    static Application *s_instance;

    Window& getWindow() { return *m_window; }

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

    /// Window
    std::unique_ptr<Window> m_window;
    /// Is the application still running
    bool m_running = false;
    /// Layer container
    LayerStack m_LayerStack;
};

Application* CreateApplication();

}