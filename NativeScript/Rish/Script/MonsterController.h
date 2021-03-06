#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Physics/Component.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

class MonsterController : public ScriptableEntity
{
public:

    enum class MonsterState
    {
        Right = 0,
        Left
    };

    MonsterState monsterState = MonsterState::Left;

    void onCreate() override;
    void onDestroy() override;

    void onUpdate(Time dt) override;
    void onImGuiRender() override;

    void setSprite(SpriteRenderComponent &render, MonsterState &monsterState);

    RL_SCRIPT_EMPTY_SERIALIZE();

private:
    // TODO: HardCode
    bool stateChange = false;
    float jumpSpeed;

};

}

RL_REGISTER_SCRIPT_TYPE(rl::MonsterController); // NOLINT
