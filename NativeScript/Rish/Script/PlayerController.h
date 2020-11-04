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

    enum class PlayerFace
    {
        Left,
        Right
    };

    enum class PlayerAction
    {
        Stand = 0,
        Ducking,
        Jump
    };

    enum class PlayerState
    {
        Alive = 0,
        Died
    };

    PlayerAction playerAction = PlayerAction::Stand;
    PlayerFace playerFace = PlayerFace::Right;

    void onCreate() override;

    void onDestroy() override;

    void onUpdate(Time dt) override;

    void onImGuiRender() override;

    void setGraphic(SpriteRenderComponent &rend, PlayerAction &playerState, PlayerFace &playerFace);

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("walkAccel", walkAccel);
        RL_SERIALIZE("walkSpeedLimit", walkSpeedLimit);
        RL_SERIALIZE("jumpSpeed", jumpSpeed);
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

};

}

RL_REGISTER_SCRIPT_TYPE(rl::PlayerController) // NOLINT
