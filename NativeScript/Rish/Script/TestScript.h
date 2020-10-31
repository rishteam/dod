#pragma once
#include <Rish/rlpch.h>
#include <Rish/Script/Common.h>

namespace rl {

class TestScript : public ScriptableEntity
{
public:
    TestScript();
    ~TestScript() override;

    void onCreate() override;
    void onDestroy() override;
    void onUpdate(Time dt) override;
    void onImGuiRender() override;

    RL_SCRIPT_EMPTY_SERIALIZE()
};

}

RL_REGISTER_SCRIPT_TYPE(rl::TestScript) // NOLINT
