#include "SandboxLayer.h"
#include "TestLayer.h"

#include <RishEngine.h>
#include <EntryPoint.h>

class Sandbox : public rl::Application
{
public:
    Sandbox() : rl::Application("Sandbox", 1920, 1080)
    {
        RL_PROFILE_FUNCTION();
//        pushLayer(new ExampleSandboxLayer());
        pushLayer(new TestLayer());
    }

    virtual ~Sandbox() override
    {
    }
};

rl::Application* rl::CreateApplication()
{
    return new Sandbox();
}
