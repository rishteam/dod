#pragma once

#include <Rish/Script/Common.h>

namespace rl {

class Cinemachine2D : public ScriptableEntity
{
public:
    void onCreate() override;
    void onDestroy() override;

    void onUpdate(Time dt) override;
    void onImGuiRender() override;

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("cameraTransSpeed", cameraTransSpeed);
        RL_SERIALIZE("startTransSpeed", startTransSpeed);
        RL_SERIALIZE("endTransSpeed", endTransSpeed);
        RL_SERIALIZE("target", chaseTarget);
        RL_SERIALIZE("targetName", chaseTargetName);
    }

private:
    float cameraTransSpeed = 2.f;
    float startTransSpeed = 1.f, endTransSpeed = 1.f;
    UUID chaseTarget;
    std::string chaseTargetName;
    float m_distance = 0.f;
};

}

RL_REGISTER_SCRIPT_TYPE(rl::Cinemachine2D) // NOLINT
