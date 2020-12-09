#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Physics/Component.h>
#include <Rish/Script/Script.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

class ParkourPlayerController : public ScriptableEntity
{
public:
    void onCreate() override;
    void onDestroy() override;
    void onUpdate(Time dt) override;
    void onImGuiRender() override;

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("walkAccel", m_walkAccel);
        RL_SERIALIZE("walkSpeedLimit", m_walkSpeedLimit);
        RL_SERIALIZE("jumpSpeed", m_jumpSpeed);
    }

private:
    float m_stopSpeed = 0.3;

    // Walk Speed
    float m_walkAccel = 5.0f;
    float m_walkSpeedLimit = 5.0f;

    // Limit Jump Times
    bool  m_prevJump = false;
    float m_jumpSpeed = 7.0f;
    int   m_jumpLimitTimes = 1;
    int   m_jumpTimesCounter = 0;

};

}

RL_REGISTER_SCRIPT_TYPE(rl::ParkourPlayerController) // NOLINT

