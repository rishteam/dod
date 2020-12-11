#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Physics/Component.h>
#include <Rish/Script/Script.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

    class Player : public ScriptableEntity
    {
    public:

        void onCreate() override;

        void onDestroy() override;

        void onUpdate(Time dt) override;

        void onImGuiRender() override;

        RL_SCRIPT_SERIALIZE()
        {
            RL_SERIALIZE("walkAccel", walkAccel);
        }
    private:

        float StopSpeed = 0.3;

        // Walk Speed
        float walkAccel = 5.0f;
        float walkSpeedLimit = 5.0f;

        // Limit Jump Times
        bool prevJump = false;
        float jumpSpeed = 7.0f;
        int jumpLimitTimes = 1;
        int jumpTimesCounter = 0;

        bool life = true;

        Clock clock;
    };

}

RL_REGISTER_SCRIPT_TYPE(rl::Player) // NOLINT
