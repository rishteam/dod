#pragma once

#include <Rish/Script/Common.h>

#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
#include <Rish/Math/Random.h>

namespace rl {

class CameraController : public ScriptableEntity
{
public:
    void onCreate() override;
    void onDestroy() override;

    void onUpdate(Time dt) override;

    void onImGuiRender() override
    {
        auto &trans = GetComponent<TransformComponent>().translate;
        ImGui::DragFloat3("Translate", glm::value_ptr(trans));
        ImGui::DragFloat("Speed", &m_speed);
    }

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("speed", m_speed);
    }

private:
    float m_speed = 10.f;
};

}

RL_REGISTER_SCRIPT_TYPE(rl::CameraController)
