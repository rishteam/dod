#pragma once

#include <Rish/Script/Common.h>

#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>
#include <Rish/Utils/String.h>

namespace rl{


class ParkourCameraController : public ScriptableEntity
{
public:

    ParkourCameraController(){
        m_followEntityID = UUID("None");
        RL_CORE_INFO("OnCreate");
    }

    void onCreate() override;
    void onDestroy() override;
    void onUpdate(Time dt) override;
    void onImGuiRender() override;

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("FollowID", m_followEntityID);
    }

private:
    UUID m_followEntityID;
};

}

RL_REGISTER_SCRIPT_TYPE(rl::ParkourCameraController) // NOLINT
