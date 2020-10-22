#pragma once

#include <Rish/Script/Common.h>

namespace rl {

class SpriteRoatate : public ScriptableEntity
{
public:
    void onUpdate(Time dt) override;

    void onImGuiRender() override
    {
    }

    RL_SCRIPT_EMPTY_SERIALIZE()
};

}

RL_REGISTER_SCRIPT_TYPE(rl::SpriteRoatate)