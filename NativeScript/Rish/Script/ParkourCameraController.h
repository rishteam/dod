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
        RL_SERIALIZE("FrameSize", m_frameSize);
    }

private:
    UUID m_followEntityID;
    float m_frameSize;
};

}

RL_REGISTER_SCRIPT_TYPE(rl::ParkourCameraController) // NOLINT
