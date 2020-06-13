#include <RishEngine.h>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

class ExampleSandbox : public rl::Layer
{
public:
    ExampleSandbox()
    : Layer("example")
    {
    }

    void onUpdate() override
    {
    }

    void onEvent(rl::Event& event) override
    {
        // RL_TRACE("ExampleLayer OnEvent: {0}", event);
        if(event.getEventType() == rl::EventType::MouseMoved)
            event.handled = true;
    }
};

class Sandbox : public rl::Application
{
public:
    Sandbox()
    {
        pushLayer(new ExampleSandbox());
        pushLayer(new rl::ImGuiLayer());
    }
    virtual ~Sandbox() override
    {
    }
};

rl::Application* rl::CreateApplication()
{
    return new Sandbox();
}
