#include "SandboxLayer.h"

#include <RishEngine.h>
#include <EntryPoint.h>

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
