/**
 * @file Application.h
 * @author roy4801 (roy@rish.com.tw), icejj
 * @brief Header of Application class
 * @date 2020-06-13
 */
#pragma once

#include "Rish/rlpch.h"

#include "Rish/Core/Core.h"
#include "Rish/Core/Window.h"
#include "Rish/Events/ApplicationEvent.h"
#include "Rish/Layer/LayerStack.h"
#include "Rish/Layer/ImGuiLayer.h"

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

    Application(const std::string &name = "Rish Engine", uint32_t width = 1280, uint32_t height = 720);
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
    /// ImGui Layer
    ImGuiLayer *m_imguiLayer;
    /// Layer container
    LayerStack m_LayerStack;
    /// Is the application still running
    bool m_running = false;
    /// previous frame time
    float m_prevFrameTime = 0.0f;

    uint32_t m_vertexArray, m_vertexBuffer, m_indexBuffer;
};

Application* CreateApplication();

}