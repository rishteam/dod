#pragma once

#include <Rish/Script/Common.h>
#include <Rish/Physics/Component.h>
#include <Rish/Script/Script.h>

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>

namespace rl {

class HpBar : public ScriptableEntity
{
public:

    void onCreate() override;

    void onDestroy() override;

    void onUpdate(Time dt) override;

    void onImGuiRender() override;

    RL_SCRIPT_SERIALIZE()
    {
    }

private:



};

}



