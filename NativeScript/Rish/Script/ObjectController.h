#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Script/Script.h>
#include <Rish/Physics/Component.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

class ObjectController : public ScriptableEntity
{
public:
    enum class ObjectState
    {
        Right = 0,
        Left
    };

    enum class ObjectType {
        Mushroom = 0,
        Star,
        Coin
    };

    ObjectState objectState = ObjectState::Right;
    ObjectType objectType = ObjectType::Mushroom;

    void onCreate() override;

    void onDestroy() override;

    void onUpdate(Time dt) override;

    void onImGuiRender() override;

    void killObject();

    RL_SCRIPT_EMPTY_SERIALIZE();

private:
    bool stateChange = false;


};



}

RL_REGISTER_SCRIPT_TYPE(rl::ObjectController); // NOLINT