#include <RishEngine.h>

class Sandbox : public rl::Application
{
public:
    Sandbox()
    {
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
