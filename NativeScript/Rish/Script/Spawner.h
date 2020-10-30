#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Physics/Component.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

class Spawner : public ScriptableEntity
{
public:
    void onCreate() override;
    void onDestroy() override;
    void onUpdate(Time dt) override;
    void onImGuiRender() override;

    RL_SCRIPT_EMPTY_SERIALIZE()
private:
    Clock clk;
    std::set<Entity> m_spawned;
};

}

RL_REGISTER_SCRIPT_TYPE(rl::Spawner) // NOLINT
