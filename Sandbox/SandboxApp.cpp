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
        RL_INFO("ExampleLayer::Update");
    }

    void onEvent(rl::Event& event) override
    {
        RL_INFO("ExampleLayer::onEvent");
        RL_TRACE("{0}", event);
    }

};

class Sandbox : public rl::Application
{
public:
    Sandbox()
    {
        pushLayer(new ExampleSandbox());
        // RL_CORE_ASSERT(false, "123");
        // RL_ASSERT(false, "456");
    }
    virtual ~Sandbox() override
    {
    }
};

rl::Application* rl::CreateApplication()
{
    return new Sandbox();
}
