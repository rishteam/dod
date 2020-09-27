#include "ParticleSystem.h"

#include <Rish/Renderer/Renderer2D.h>

std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

ParticleSystem::ParticleSystem(uint32_t maxParticles)
    : m_poolStartIndex(maxParticles-1)
{
    m_pool.resize(maxParticles);
}

void ParticleSystem::onUpdate(rl::Time dt)
{
    // 更新 pool 中的所有 particle
    for(auto & p : m_pool)
    {
        if(!p.active) // 如果沒有 active 則不做
            continue;
        // particle 的生命結束了
        if(p.lifeRemaining <= 0.f)
        {
            p.active = false;
            continue;
        }
        // 更新 剩餘生命、位置、旋轉度度
        p.lifeRemaining -= dt;
        p.position += p.velocity * (float)dt;
        p.rotation += 1.f * dt;
    }
}

void ParticleSystem::onRender(rl::OrthographicCamera &camera)
{
    rl::Renderer2D::BeginScene(camera, true);
    for(auto &p : m_pool)
    {
        if(!p.active) continue;

        float remainLifeRatio = p.lifeRemaining / p.lifeTime;
        glm::vec4 curColor = glm::lerp(p.colorEnd, p.colorBegin, remainLifeRatio);
//        curColor.a *= remainLifeRatio;

        float curSize = glm::lerp(p.sizeEnd, p.sizeBegin, remainLifeRatio);

        // render
        rl::Renderer2D::DrawRotatedQuad(p.position, {curSize, curSize}, curColor, p.rotation);
    }
    rl::Renderer2D::EndScene();
}

void ParticleSystem::emit(const ParticleSetting &setting)
{
    // Init a Particle
    Particle& particle = m_pool[m_poolStartIndex];
    particle.active = true;
    particle.position = setting.position;
    particle.rotation = Random::Float() * 360.f; // [0, 2pi]

    // Velocity
    particle.velocity = setting.velocity;
    // 讓每個 particle 的速度都有些許不同
    particle.velocity.x += setting.velocityVar.x * (Random::Float() - 0.5f); //
    particle.velocity.y += setting.velocityVar.y * (Random::Float() - 0.5f);

    // Color
    particle.colorBegin = setting.colorBegin;
    particle.colorEnd = setting.colorEnd;
    // Life
    particle.lifeTime = setting.lifeTime;
    particle.lifeRemaining = setting.lifeTime;
    // 讓每個 particle 一開始的大小都有些許不同
    particle.sizeBegin = setting.sizeBegin + setting.sizeVar * (Random::Float() - 0.5f);
    particle.sizeEnd = setting.sizeEnd;

    m_poolStartIndex--;
    if(m_poolStartIndex <= 0)
        m_poolStartIndex = m_pool.size() - 1;
}
