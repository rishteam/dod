#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Physics/Component.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

class PlayerController : public ScriptableEntity
{
public:
    void onCreate() override;

    void onDestroy() override;

    void onUpdate(Time dt) override;

    void onImGuiRender() override;

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("x_force_speed", x_force_speed);
        RL_SERIALIZE("y_force_speed", y_force_speed);
    }
private:
    float x_force_speed;
    float y_force_speed;

};

}

RL_REGISTER_SCRIPT_TYPE(rl::PlayerController)
