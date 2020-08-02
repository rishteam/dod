#include <Rish/Core/Core.h>
#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>

#include <Rish/Events/Event.h>
#include <Rish/Events/ApplicationEvent.h>

#include <Rish/Renderer/Renderer.h>

#include <spdlog/fmt/bundled/printf.h>
#include <spdlog/fmt/bundled/format.h>

namespace rl {

Application *Application::s_instance = nullptr;

Application::Application(const std::string &name, uint32_t width, uint32_t height)
{
    RL_PROFILE_FUNCTION();
    RL_CORE_ASSERT(s_instance == nullptr, "RishEngine should only have *ONE* Application instance");
    // Set instance
    Application::s_instance = this;

    // Initialize the window
    m_window = Scope<Window>(Window::Create(name, width, height));
    m_window->setEventCallback(RL_BIND_EVENT_FUNC(Application::onEvent));

    // Initialize the renderer
    Renderer::Init();

    // imgui overlay
    m_imguiLayer = new ImGuiLayer();

    // Initialize Layer Stack
    m_LayerStack = MakeScope<LayerStack>();
    m_LayerStack->pushOverlay(m_imguiLayer); // Gives the ownership of m_imguiLayer layer to m_LayerStack

    m_running = true;
}

Application::~Application()
{
    RL_PROFILE_FUNCTION();
    // pop all layers
    m_LayerStack.reset(nullptr);
    //
    Renderer::Shutdown();
    //
    m_window.reset(nullptr);
}

void Application::run()
{
    RL_PROFILE_FUNCTION();
    Clock clk;
    float lag = 0.0;

    while(m_running)
    {
        RL_PROFILE_SCOPE("Main Loop");
        // Update time
        float now = clk.getElapsedTime().asSeconds();
        Time dt = now - m_prevFrameTime;
        m_prevFrameTime = now;
        lag += dt;

        // Update window
        m_window->onUpdate();

        // Update Timer
        Timer::Update();

        // Update layers
        for(Layer* layer: *m_LayerStack)
            layer->onUpdate(dt);

        // Update ImGui
        m_imguiLayer->begin();
        for(Layer* layer : *m_LayerStack)
            layer->onImGuiRender();
        m_imguiLayer->end();

        // Draw window
        if(m_window)
            m_window->onDraw();
    }
}

void Application::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(RL_BIND_EVENT_FUNC(Application::onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(RL_BIND_EVENT_FUNC(Application::onWindowResize));

    for(auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); it++)
    {
        (*it)->onEvent(e);
        // if event is handled then break
        if(e.handled)
            break;
    }
}

void Application::pushLayer(Layer* layer)
{
    RL_CORE_ASSERT(layer != nullptr, "null layer");
    m_LayerStack->pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay)
{
    RL_CORE_ASSERT(overlay != nullptr, "null overlay");
    m_LayerStack->pushOverlay(overlay);
}

void Application::popLayer(Layer* layer)
{
    RL_CORE_ASSERT(layer != nullptr, "null layer");
    m_LayerStack->popLayer(layer);
}

void Application::popOverlay(Layer* overlay)
{
    RL_CORE_ASSERT(overlay != nullptr, "null overlay");
    m_LayerStack->popOverlay(overlay);
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
    Renderer::OnWindowResize(e.m_width, e.m_height);
    return false;
}

}
