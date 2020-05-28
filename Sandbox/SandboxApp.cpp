#include <RishEngine.h>

class ExampleSandbox : public rl::Layer
{
public:
    ExampleSandbox()
    : Layer("example")
    {
    }

    void onUpdate() override
    {
        RL_TRACE("ExampleLayer onUpdate");
    }

    void onEvent(rl::Event& event) override
    {
        RL_TRACE("ExampleLayerOnEvent: {0}", event);
    }

};

class Sandbox : public rl::Application
{
public:
    Sandbox()
    {
        pushLayer(new ExampleSandbox());
    }
    virtual ~Sandbox() override
    {
    }
};

rl::Application* rl::CreateApplication()
{
    return new Sandbox();
}
