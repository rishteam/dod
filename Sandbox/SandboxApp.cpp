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
        // RL_TRACE("ExampleLayer onUpdate");
    }

    void onEvent(rl::Event& event) override
    {
        // RL_TRACE("ExampleLayerOnEvent: {0}", event);
        if(event.getEventType() == rl::EventType::MouseMoved)
            event.handled = true;
    }
};

class Test : public rl::Layer
{
public:
    Test()
        : Layer("test")
    {
    }

    void onEvent(rl::Event &event) override
    {
        // RL_TRACE("Test OnEvent: {0}", event);
    }
};

class Sandbox : public rl::Application
{
public:
    Sandbox()
    {
        pushLayer(new Test());
        pushLayer(new ExampleSandbox());

        rl::VFS::Get()->Mount("data", "data/");
        rl::VFS::Get()->Mount("data", "a/b/c/");
        std::string s;
        rl::VFS::Get()->ResolvePhysicalPath("data/a.jpg", s);
        RL_TRACE("{}", s);
    }
    virtual ~Sandbox() override
    {
    }
};

rl::Application* rl::CreateApplication()
{
    return new Sandbox();
}
