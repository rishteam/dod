#include <RishEngine.h>

class Sandbox : public rl::Application
{
public:
    Sandbox()
    {
    }
    virtual ~Sandbox() override
    {
    }
};

rl::Application* rl::CreateApplication()
{
    return new Sandbox();
}
