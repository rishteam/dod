#pragma once

#include <Rish/Script/Common.h>

#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>
#include <Rish/Utils/String.h>

namespace rl {

class ParkourObjMove : public ScriptableEntity
{

public:
    void onCreate() override;
    void onDestroy() override;
    void onUpdate(Time dt) override;
    void onImGuiRender() override;

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("nowMove", m_nowMove);
        RL_SERIALIZE("counter",m_counter);
        RL_SERIALIZE("move", m_move);
    }

private:

    int m_nowMove;
    float m_counter;
    std::vector <std::pair<glm::vec2,float>> m_move;

};

}

RL_REGISTER_SCRIPT_TYPE(rl::ParkourObjMove) // NOLINT
