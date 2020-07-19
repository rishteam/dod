#include "Rish/Core/Core.h"
#include "Rish/Core/Application.h"
#include "Rish/Core/Time.h"

#include "Rish/Events/Event.h"
#include "Rish/Events/ApplicationEvent.h"

#include <fmt/printf.h>
#include <fmt/format.h>

#include <SFML/System/Clock.hpp>
#include <SFML/OpenGL.hpp>
#include <imgui.h>

// #include "Rish/Renderer/Buffer.h"
// #include "Rish/Renderer/VertexArray.h"

namespace rl {

#define S_PER_UPDATE 0.01

Application *Application::s_instance = nullptr;

Application::Application(const std::string &name, uint32_t width, uint32_t height)
{
    RL_CORE_ASSERT(s_instance == nullptr, "RishEngine should only have ONE Application instance");
    Application::s_instance = this; // set instance
    m_window = std::unique_ptr<Window>(Window::Create(name, width, height));
    m_window->setEventCallback(RL_BIND_EVENT_FUNC(Application::onEvent));
    m_running = true; // set the running flag
    // Push the imgui overlay
    m_imguiLayer = new ImGuiLayer();
    pushOverlay(m_imguiLayer); 

    m_vertexArray.reset(new VertexArray());

    float vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };

    std::shared_ptr<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(new VertexBuffer(vertices, sizeof(vertices)));

    BufferLayout layout = {

        { ShaderDataType::Float3, "a_Position"},
        { ShaderDataType::Float4, "a_Color"}
    };

    vertexBuffer->setLayout(layout);
    m_vertexArray->addVertexBuffer(vertexBuffer);

    uint32_t indices[3] = { 0, 1, 2 };
    std::shared_ptr<IndexBuffer> indexBuffer;
    indexBuffer.reset(new IndexBuffer(indices, sizeof(indices)/sizeof(uint32_t)));
    m_vertexArray->setIndexBuffer(indexBuffer);

    m_vertexArray->unbind(); // Always remember to UNBIND if AMD

    testVA.reset(new VertexArray());

    float square[3 * 4] = {
        -0.75f, -0.75f, 0.0f,
         0.75f, -0.75f, 0.0f,
         0.75f,  0.75f, 0.0f,
        -0.75f,  0.75f, 0.0f
    };

    std::shared_ptr<VertexBuffer> squareVB;
    squareVB.reset(new VertexBuffer(square, sizeof(square)));
    squareVB->setLayout({

        { ShaderDataType::Float3, "a_Position" }
    });
    testVA->addVertexBuffer(squareVB);

    uint32_t squareIndices[6] = { 0, 1, 2, 2, 0, 3};
    std::shared_ptr<IndexBuffer> squareIB;
    squareIB.reset(new IndexBuffer(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
    testVA->setIndexBuffer(squareIB);

    testVA->unbind();
    std::string vertexSrc = R"(

        #version 330 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            gl_Position = vec4(a_Position, 1.0);
            v_Position = a_Position;
            v_Color = a_Color;
        }
    )";

    std::string fragSrc = R"(

        #version 330 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main()
        {
            color = vec4(v_Position * 0.5 + 0.5, 1.0);
            color = v_Color;
        }   
    )";

    m_shader.reset(new Shader(vertexSrc, fragSrc));

    std::string blueShaderVertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;

        out vec3 v_Position;

        void main()
        {
            v_Position = a_Position;
            gl_Position = vec4(a_Position, 1.0);	
        }
    )";

    std::string blueShaderFragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;

        in vec3 v_Position;

        void main()
        {
            color = vec4(0.2, 0.3, 0.8, 1.0);
        }
    )";

    testShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
}

Application::~Application()
{
}

void Application::run()
{
    // static sf::Clock clk; // TODO(roy4801): make RishEngine clock in the future
    Clock clk;
    float lag = 0.0;

    while(m_running)
    {
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
        for(Layer* layer: m_LayerStack)
            layer->onUpdate(dt);

        // Update ImGui
        m_imguiLayer->begin();
        for(Layer* layer : m_LayerStack)
            layer->onImGuiRender();
        m_imguiLayer->end();

        testShader->bind();
        testVA->bind();
        glDrawElements(GL_TRIANGLES, testVA->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
        testVA->unbind();
        
        m_shader->bind();
        m_vertexArray->bind();
        glDrawElements(GL_TRIANGLES, m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
        m_vertexArray->unbind();


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

    for(auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
    {
        (*it)->onEvent(e);
        // if event is handled then break
        if(e.handled)
            break;
    }
}

void Application::pushLayer(Layer* layer)
{
    m_LayerStack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay)
{
    m_LayerStack.pushOverlay(overlay);
}

void Application::popLayer(Layer* layer)
{
    m_LayerStack.popLayer(layer);
}

void Application::popOverlay(Layer* overlay)
{
    m_LayerStack.popOverlay(overlay);
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
    glViewport(0, 0, e.m_width, e.m_height);
    return false;
}

}
