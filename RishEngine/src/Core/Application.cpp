#include <Rish/Core/Application.h>
#include <Rish/Core/Core.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
//
#include <Rish/Core/FileSystem.h>
#include <Rish/Core/VFS.h>
//
#include <Rish/Events/Event.h>
#include <Rish/Events/ApplicationEvent.h>
//
#include <Rish/Renderer/Renderer.h>
//
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>

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

    VFS::Mount("shader", "assets/shader");
    VFS::Mount("fonts", "assets/fonts");
    VFS::Mount("texture", "assets/texture");

    ScriptableManager::Register<EmptyScript>();

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
    ScriptableManager::Shutdown();
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
        static int loopCnt = 0;
        static std::string loopLabel = "Main Loop ";
        RL_PROFILE_SCOPE((loopLabel + std::to_string(loopCnt++)).c_str());

        // Update time
        float now = clk.getElapsedTime().asSeconds();
        Time dt = now - m_prevFrameTime;
        m_prevFrameTime = now;
        lag += dt;

        if(lag >= 1.f)
        {
            m_fps = m_frameCount;
            m_frameCount = 0;
            lag -= 1.f;
        }

        // Update window
        {
            RL_PROFILE_SCOPE("Window Update");
            m_window->onUpdate();
        }

        // Update Timer
        Timer::Update();

        // Update layers
        {
            RL_PROFILE_SCOPE("Layer Update");
            for(Layer* layer: *m_LayerStack)
                layer->onUpdate(dt);
        }

        // TODO: Due to some random fucking OpenGL shit states and SFML shits
        // so we need this here
        RenderCommand::ClearStates();

        // Update ImGui
        {
            RL_PROFILE_SCOPE("ImGui Update");
            m_imguiLayer->begin();
            for (Layer *layer : *m_LayerStack)
                layer->onImGuiRender();
            m_imguiLayer->end();
        }

        // Draw window
        {
            RL_PROFILE_SCOPE("Draw");
            if (m_window)
                m_window->onDraw();
        }

        m_frameCount++;
    }
}

void Application::onEvent(Event &e)
{
    RL_PROFILE_FUNCTION();

    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(RL_BIND_EVENT_FUNC(Application::onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(RL_BIND_EVENT_FUNC(Application::onWindowResize));
    dispatcher.dispatch<WindowFocusEvent>(RL_BIND_EVENT_FUNC(Application::onWindowFocus));
    dispatcher.dispatch<WindowLostFocusEvent>(RL_BIND_EVENT_FUNC(Application::onWindowLostFocus));

    if(!m_isWindowFocus)
    {
        if(e.isInCategory(EventCategoryInput))
            e.handled = true;
    }

    for(auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); it++)
    {
        // if event is handled then break
        if(e.handled)
            break;
        (*it)->onEvent(e);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Application Layer Operations
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// Application Event Handlers
////////////////////////////////////////////////////////////////////////////////

bool Application::onWindowClose(WindowCloseEvent &e)
{
    m_running = false;
    return true;
}

bool Application::onWindowResize(WindowResizeEvent &e)
{
    RL_CORE_TRACE("{}", e.toString());
    Renderer::OnWindowResize(e.m_width, e.m_height);
    return false;
}

bool Application::onWindowFocus(WindowFocusEvent &e)
{
    RL_UNUSED(e);
    m_isWindowFocus = true;
    Input::SetInputState(true);
    return false;
}

bool Application::onWindowLostFocus(WindowLostFocusEvent &e)
{
    RL_UNUSED(e);
    m_isWindowFocus = false;
    Input::SetInputState(false);
    return false;
}

}
