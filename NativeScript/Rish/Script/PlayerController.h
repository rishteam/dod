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

    enum class PlayerState
    {
        Stand = 0,
        Ducking,
        Jump
    };

    PlayerState playerState = PlayerState::Stand;
    PlayerFace playerFace = PlayerFace::Right;

    void onCreate() override;

    void onDestroy() override;

    void onUpdate(Time dt) override;

    void onImGuiRender() override;

    void setGraphic(SpriteRenderComponent &rend, PlayerState &playerState, PlayerFace &playerFace);

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("walkAccel", walkAccel);
        RL_SERIALIZE("walkSpeedLimit", walkSpeedLimit);
        RL_SERIALIZE("jumpSpeed", jumpSpeed);
    }
private:
    float currentMaxMove = 5.0f;
    float jumpSpeed;

    float walkAccel;
    float walkSpeedLimit;

    int jumpLimitTimes = 1;
    int jumpTimesCounter = 0;
    bool prevJump = false;
    bool stillJumping = false;

};

}

RL_REGISTER_SCRIPT_TYPE(rl::PlayerController) // NOLINT
