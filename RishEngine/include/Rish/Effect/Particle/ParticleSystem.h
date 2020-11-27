#pragma once

#include <Rish/rlpch.h>

#include <entt/entt.hpp>

namespace rl {

class Scene;
class ParticleComponent;
class TransformComponent;

class RL_API ParticleSystem
{
public:
    enum VortexType
    {
        StaticVortex = 0,
        DynamicVortex
    };

    static void RegisterScene(const Ref<Scene> &scene);

    static void onUpdate(float dt);
    static void onRender();
    static void onEditorRender(bool isSimulate);

private:
    static Ref<Scene> s_Scene;

    // This function exists because it's written before the Math::Random library had been written
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
