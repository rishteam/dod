#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Physics/Component.h>


#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>
namespace rl {

class EventBoxController : public ScriptableEntity
{
public:
    void onCreate() override;

    void onDestroy() override;

    void onUpdate(Time dt) override;

    void onImGuiRender() override;
};

}

RL_REGISTER_SCRIPT_TYPE(rl::EventBoxController);
