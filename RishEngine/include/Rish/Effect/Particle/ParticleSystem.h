#pragma once

#include <Rish/Core/Core.h>
#include <Rish/Core/Log.h>
#include <Rish/Scene/Component.h>
#include <Rish/Renderer/Camera/OrthographicCameraController.h>

#include <entt/entt.hpp>

namespace rl {

class RL_API ParticleSystem {

public:
    ParticleSystem() = default;

    static void onUpdate(entt::registry& registry, float dt);

    static void render(entt::registry& registry);

    static void initEmitter();

private:

    OrthographicCameraController m_cameraController;

    static float randomFloat(float min, float max);

    static uint32_t firstUnusedParticle(entt::registry& registry, entt::entity& entity);

    static void respawnParticle(ParticleComponent& emitter, TransformComponent& transform, int unusedParticle);

    static uint32_t firstUnusedVortex(entt::registry& registry, entt::entity& entity);

    static void respawnVortex(ParticleComponent& emitter, TransformComponent& transform, int unusedVortex);

    static float interpolateBetweenRange(float min, float timestep, float max);

    static glm::vec4 RGBAInterpolation(glm::vec4 startColor, float timestep, glm::vec4 endColor);

};




}




