#pragma once

#include <Rish/rlpch.h>
#include <Rish/Scene/Component.h>
#include <Rish/Scene/Scene.h>

#include <entt/entt.hpp>

namespace rl {

class RL_API ParticleSystem
{
public:
    ParticleSystem() = default;

    static void onUpdate(entt::registry& registry, float dt);
    static void onRender(entt::registry& registry, Scene::SceneState state = Scene::SceneState::Editor);

    static bool loadEmitData();

private:
    // TODO: remove
    static float randomFloat(float min, float max);

    // Particle
    static uint32_t firstUnusedParticle(entt::registry& registry, entt::entity& entity);
    static void respawnParticle(ParticleComponent& emitter, TransformComponent& transform, int unusedParticle);

    // Vertex
    static uint32_t firstUnusedVortex(entt::registry& registry, entt::entity& entity);
    static void respawnVortex(ParticleComponent& emitter, TransformComponent& transform, int unusedVortex);

    // Math
    static float interpolateBetweenRange(float min, float timestep, float max);
    static glm::vec4 RGBAInterpolation(glm::vec4 startColor, float timestep, glm::vec4 endColor);
};

}
