#pragma once

#include <Rish/rlpch.h>

#include <Rish/Core/Time.h>
#include <Rish/Renderer/Camera/OrthographicCamera.h>

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    static float Float()
    {
        return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }

private:
    static std::mt19937 s_RandomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

struct ParticleSetting
{
    glm::vec2 position;
    glm::vec2 velocity, velocityVar;
    glm::vec4 colorBegin, colorEnd;
    float sizeBegin, sizeEnd, sizeVar;
    float lifeTime = 1.f; // seconds
};

class ParticleSystem
{
public:
    ParticleSystem(uint32_t maxParticles);

    void onUpdate(rl::Time dt);
    void onRender(rl::OrthographicCamera &camera);

    void emit(const ParticleSetting &setting);

private:
    struct Particle
    {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec4 colorBegin, colorEnd;
        float rotation = 0.0f; // in radians
        float sizeBegin, sizeEnd;

        float lifeTime = 1.0f;
        float lifeRemaining = 0.0f;

        bool active = false;
    };
    std::vector<Particle> m_pool;
    uint32_t m_poolStartIndex;
};
