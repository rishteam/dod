#pragma once
#include <Rish/Script/Common.h>
#include <Rish/Script/Script.h>
#include <Rish/Physics/Component.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

class BoxEventController : public ScriptableEntity
{
public:

    enum class EventContent {
        Empty = 0,
        Mushroom,
        Star,
        Coin
    };

    EventContent eventContent = EventContent::Empty;
    int contentCount = 1;

    void onCreate() override;

    void onDestroy() override;

    void spawnObject(EventContent eventTrigger);

    void onUpdate(Time dt) override;

    void onImGuiRender() override;

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("eventContent", eventContent);
    }
};

}

RL_REGISTER_SCRIPT_TYPE(rl::BoxEventController); // NOLINT
