#include <RishEngine.h>

#include <imgui.h>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

class ExampleSandboxLayer : public rl::Layer
{
public:
    ExampleSandboxLayer()
    : Layer("example")
    {
    }

    void onUpdate(rl::Time dt) override
    {
        static rl::Time tmp;
        tmp += dt;
        RL_INFO("dt={}", tmp);
    }

    void onImGuiRender() override
    {
        ImGui::Begin("aa");
        ImGui::Text("123");
        ImGui::End();

        ImGui::Begin("bb");
        ImGui::Text("456");
        ImGui::End();

        ImGui::Begin("cc");
        ImGui::Text("789");
        ImGui::End();

        ImGui::Begin("Console");
        ImGui::Text("aaaaa");
        ImGui::Text("bbbbb");
        ImGui::End();
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
    Sandbox() : rl::Application("Sandbox", 1920, 1080)
    {
        pushLayer(new ExampleSandboxLayer());
    }
    virtual ~Sandbox() override
    {
    }
};

rl::Application* rl::CreateApplication()
{
    return new Sandbox();
}
