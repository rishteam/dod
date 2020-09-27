#include <Rish/Effect/Particle/ParticleSystem.h>
#include <Rish/Renderer/Renderer2D.h>

namespace rl {

void ParticleSystem::onUpdate(entt::registry &registry, float dt)
{
    RL_CORE_TRACE("ParticleSystem onUpdate");

    auto view = registry.view<TransformComponent, ParticleComponent>();
    for(auto entity : view)
    {
        auto &emitter   = registry.get<ParticleComponent>(entity);
        auto &transform = registry.get<TransformComponent>(entity);

        // Dirty Flag
        // if change EmitNumber/EmitVariance on ImGui then recal
        // TODO Move to other place
        if(emitter.poolSize != (emitter.emitNumber + emitter.emitVariance) * (emitter.maxParticleLife + 1))
        {
            emitter.poolSize = (emitter.emitNumber + emitter.emitVariance) * (emitter.maxParticleLife + 1);
            emitter.particles.clear();
            emitter.particles.resize(emitter.poolSize);
            emitter.lastUnusedParticle = 0;
        }

        if(emitter.vortexSensitive && (emitter.vortexType == DYNAMIC_VORTEX)) {

            if(emitter.vortexPoolSize != emitter.vortexEmitNumber*3){

                emitter.vortexPoolSize = emitter.vortexEmitNumber*3;
                emitter.dynamic_vortexes.clear();
                emitter.dynamic_vortexes.resize(emitter.vortexPoolSize);
                emitter.lastUnusedVortex = 0;
            }
        }

        // TODO: remove me
        if(emitter.loadTexure)
        {
            emitter.texture = Texture2D::LoadTextureVFS(emitter.texturePath);
            emitter.loadTexure = false;
        }


        if (emitter.active)
        {
            // cal the num that should respawn this frame
            emitter.emissionRate = (int)(emitter.emitNumber + emitter.emitVariance * randomFloat(emitter.emitVarianceRand.x, emitter.emitVarianceRand.y));

            for(int i = 0 ; i < emitter.emissionRate ; i++)
            {
                // find the unused Particle
                int unusedParticle = firstUnusedParticle(registry, entity);
                // 激活他
                respawnParticle(emitter, transform, unusedParticle);
            }

            if(emitter.vortexSensitive && (emitter.vortexType == DYNAMIC_VORTEX))
            {
                emitter.vortexEmissionRate = emitter.vortexEmitNumber;

                if(emitter.vortexActive)
                {
                    for(int i = 0 ; i < emitter.vortexEmissionRate ; i++)
                    {
                        int unusedVortex = firstUnusedVortex(registry, entity);
                        if(unusedVortex == -1)
                            break;
                        respawnVortex(emitter, transform, unusedVortex);
                    }
                }
            }
        }

        if(emitter.vortexSleepTime > 0.f)
        {
            if(emitter.vortexSleepTimer.getElapsedTime() <= emitter.vortexSleepTime)
            {
                emitter.vortexActive = false;
            }
            else
            {
                emitter.vortexActive = true;
                emitter.vortexSleepTimer.restart();
            }
        }

        // Update particles' states by sleep time and life time
        if(emitter.sleepTime > 0.f)
        {
            if(emitter.sleepTimer.getElapsedTime() <= emitter.sleepTime)
            {
                emitter.active = false;
            }
            else
            {
                emitter.active = true;
                emitter.sleepTimer.restart();
            }
        }

        // if emitter still has life
        // for ImGui change
        // TODO Move to other place
        if(emitter.life > 0.f)
        {
            // exceeded
            if(emitter.lifeTimer.getElapsedTime() >= emitter.life)
            {
                emitter.active = false;
            }

            if(emitter.life != emitter.life_store)
            {
                emitter.active = true;
                emitter.life_store = emitter.life;
                emitter.lifeTimer.restart();
            }
        }

        // Update Dynamic vortex
        if(emitter.vortexType == DYNAMIC_VORTEX && emitter.vortexSensitive)
        {
            for(auto &vortex: emitter.dynamic_vortexes)
            {
                if(vortex.life > 0.f)
                {
                    vortex.currentSize  = interpolateBetweenRange(vortex.startSize, vortex.timeStep, vortex.endSize);
                    vortex.currentVel.x = interpolateBetweenRange(vortex.startVel.x, vortex.timeStep, vortex.endVel.x);
                    vortex.currentVel.y = interpolateBetweenRange(vortex.startVel.y, vortex.timeStep, vortex.endVel.y);

                    vortex.pos.x += vortex.currentVel.x * dt;
                    vortex.pos.y += vortex.currentVel.y * dt;

                    vortex.timeStep += (1.0f/(float)vortex.startLife);
                    if(vortex.timeStep >= 1.f)
                        vortex.timeStep = 0.f;
                    vortex.life--;
                }
            }
        }

        // Update particles
        for(int i = 0 ; i < emitter.poolSize ; i++)
        {
            auto &particle = emitter.particles[i];
            if(particle.life > 0.f) {
                // Update
                // Interpolate values
                // 利用差質法算出目前的資訊
                particle.currentSize  = interpolateBetweenRange(particle.startSize, particle.timeStep, particle.endSize);
                particle.currentVel.x = interpolateBetweenRange(particle.startVel.x, particle.timeStep, particle.endVel.x);
                particle.currentVel.y = interpolateBetweenRange(particle.startVel.y, particle.timeStep, particle.endVel.y);
                particle.currentColor = RGBAInterpolation(particle.startColor, particle.timeStep, particle.endColor);

                // 計算受vortex影響的結果

                // multi vortex
                float factor_ = 0;
                float vortexEffectX = 0;
                float vortexEffectY = 0;

                if(emitter.vortexSensitive)
                {
                    if(emitter.vortexType == STATIC_VORTEX)
                    {
                        for(auto vortex : emitter.static_vortexes)
                        {
                            if(vortex.currentSize == 0.f)
                                continue;

                            float dx = particle.pos.x - vortex.pos.x;
                            float dy = particle.pos.y - vortex.pos.y;
                            float vx = -dy * vortex.turbulence.x;
                            float vy = dx * vortex.turbulence.y;
                            factor_ = 1.0f/ (1.0f + (dx*dx + dy*dy)/vortex.currentSize*0.1);

                            vortexEffectX += (vx - particle.currentVel.x) * factor_;
                            vortexEffectY += (vy - particle.currentVel.y) * factor_;
                        }
                    }

                    if(emitter.vortexType == DYNAMIC_VORTEX)
                    {
                        for(auto vortex : emitter.dynamic_vortexes)
                        {
                            if(vortex.life > 0.f)
                            {
                                float dx = particle.pos.x - vortex.pos.x;
                                float dy = particle.pos.y - vortex.pos.y;
                                float vx = -dy * vortex.turbulence.x;
                                float vy = dx * vortex.turbulence.y;
                                factor_ = 1.0f/ (1.0f + (dx*dx + dy*dy)/(vortex.currentSize*0.1));
                                float lifeFactor = vortex.life/emitter.vortexMaxParticleLife;
                                factor_ *= (1-lifeFactor)*lifeFactor*4;

                                vortexEffectX += (vx - particle.currentVel.x) * factor_;
                                vortexEffectY += (vy - particle.currentVel.y) * factor_;
                            }
                        }
                    }
                }

                particle.pos.x += vortexEffectX + particle.currentVel.x * dt;
                particle.pos.y += vortexEffectY + particle.currentVel.y * dt;
                particle.life--;
                // 計算差值的time
                particle.timeStep+= (1.0f/(float)particle.startLife);
                if(particle.timeStep >= 1.f)
                    particle.timeStep = 0.f;
                //
                // 計算rotate速度跟角度
                particle.currentRotSpeed += particle.startRotSpeed;
                particle.angle += dt * particle.currentRotSpeed;
                particle.angle = fmod(particle.angle, 360.f);
            }
        }
    }
}

void ParticleSystem::onRender(entt::registry &registry)
{
    auto view = registry.view<TransformComponent, ParticleComponent>();

    for (auto &entity: view)
    {
        auto &transform = registry.get<TransformComponent>(entity);
        auto &emitter   = registry.get<ParticleComponent>(entity);

        for(auto &particle: emitter.particles)
        {
            Renderer2D::DrawRotatedQuad(particle.pos, {particle.currentSize, particle.currentSize}, emitter.texture,
                                        particle.currentColor, glm::radians(particle.angle));
//            RL_CORE_TRACE("{}", particle.pos.x);
        }
        // TODO Vortex render;
        if(emitter.vortexSensitive)
        {
            if(emitter.vortexType == STATIC_VORTEX)
            {
                for(auto vortex : emitter.static_vortexes)
                {
                    if(vortex.draw)
                    {
                        Renderer2D::DrawLine(vortex.pos, vortex.pos+glm::vec2{(vortex.currentVel.x < 0 ? -1 : 1)*vortex.currentSize, 0.f}, glm::vec4{0.f, 0.f, 1.f, 1.f});
                        Renderer2D::DrawLine(vortex.pos, vortex.pos+glm::vec2{0.f, (vortex.currentVel.y < 0 ? -1 : 1)*vortex.currentSize}, glm::vec4{0.f, 1.f, 0.f, 1.f});
                    }
                }
            }

            if(emitter.vortexType == DYNAMIC_VORTEX && emitter.drawVortex)
            {
                for(auto vortex : emitter.dynamic_vortexes)
                {
                    if(vortex.life > 0.f)
                    {
                        Renderer2D::DrawLine(vortex.pos, vortex.pos+glm::vec2{(vortex.currentVel.x < 0 ? -1 : 1)*vortex.currentSize, 0.f}, glm::vec4{1.f, 0.f, 1.f, 1.f});
                        Renderer2D::DrawLine(vortex.pos, vortex.pos+glm::vec2{0.f, (vortex.currentVel.y < 0 ? -1 : 1)*vortex.currentSize}, glm::vec4{0.f, 1.f, 0.f, 1.f});
                    }
                }
            }
        }
    }
}

float ParticleSystem::randomFloat(float min, float max)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float r = random * (max - min);

    return min + r;
}

uint32_t ParticleSystem::firstUnusedParticle(entt::registry &registry, entt::entity& entity)
{
    auto &emitter = registry.get<ParticleComponent>(entity);

    for(int i = emitter.lastUnusedParticle ; i < emitter.poolSize ; i++)
    {
        if(emitter.particles[i].life <= 0)
        {
            emitter.lastUnusedParticle = i;
            return i;
        }
    }

    for(int i = 0 ; i < emitter.lastUnusedVortex ; i++)
    {
        if(emitter.particles[i].life <= 0)
        {
            emitter.lastUnusedParticle = i;
            return i;
        }
    }

    emitter.lastUnusedParticle = 0;
    return emitter.lastUnusedParticle;
}

void ParticleSystem::respawnParticle(ParticleComponent &emitter, TransformComponent &transform, int unusedParticle)
{
    auto &particle = emitter.particles[unusedParticle];

    float startSpeed         = emitter.startSpeed * randomFloat(emitter.startSpeedRand.x, emitter.startSpeedRand.y);
    float endSpeed           = emitter.endSpeed   * randomFloat(emitter.endSpeedRand.x, emitter.endSpeedRand.y);
    float randAngle          = randomFloat(emitter.angleRange.x, emitter.angleRange.y);
    float randStartSize      = emitter.startSize  * randomFloat(emitter.startSizeRand.x, emitter.startSizeRand.y);
    float randEndSize        = emitter.endSize    * randomFloat(emitter.endSizeRand.x, emitter.endSizeRand.y);
    float randRadius         = randomFloat(randStartSize, randEndSize/2);
    float randLife           = emitter.maxParticleLife * randomFloat(emitter.lifeRand.x, emitter.lifeRand.y);
    float randRotateSpeed    = emitter.rotateSpeed * randomFloat(emitter.rotSpeedRand.x, emitter.rotSpeedRand.y);
    float randRotAngle       = randomFloat(0.f, emitter.rotateSpeed == 0? 0.f : 360.f);
    glm::vec3 distance       = {randomFloat(emitter.disX, -emitter.disX), randomFloat(emitter.disX, -emitter.disY), 0.f};

    particle.pos             = transform.translate + distance;
    particle.startVel        = {startSpeed * cos(glm::radians(randAngle)), startSpeed * sin(glm::radians(randAngle))};
    particle.endVel          = {endSpeed   * cos(glm::radians(randAngle)), endSpeed   * sin(glm::radians(randAngle))};
    particle.angle           = randRotAngle;
    particle.startRotSpeed   = particle.currentRotSpeed = randRotateSpeed;
    particle.life            = particle.startLife       = randLife;
    particle.currentSize     = particle.startSize       = randRadius;
    particle.timeStep        = 0.f;

    particle.startColor      = emitter.startColor;
    particle.endColor        = emitter.endColor;

    particle.vortexSensitive = emitter.vortexSensitive;
}

uint32_t ParticleSystem::firstUnusedVortex(entt::registry &registry, entt::entity& entity)
{
    auto &emitter = registry.get<ParticleComponent>(entity);
    for(int i = emitter.lastUnusedVortex ; i < emitter.vortexPoolSize ; i++)
    {
        if(emitter.dynamic_vortexes[i].life <= 0)

        {
            emitter.lastUnusedVortex = i;
            return i;
        }
    }

    for(int i = 0 ; i < emitter.lastUnusedVortex ; i++)
    {
        if(emitter.dynamic_vortexes[i].life <= 0)
        {
            emitter.lastUnusedVortex = i;
            return i;
        }
    }

    emitter.lastUnusedVortex = 0;
    return -1;
}

void ParticleSystem::respawnVortex(ParticleComponent &emitter, TransformComponent &transform, int unusedVortex)
{
    auto &vortex = emitter.dynamic_vortexes[unusedVortex];

    float randomAngle = randomFloat(emitter.vortexAngleRange.x, emitter.vortexAngleRange.y);
    float startSpeed  = emitter.vortexStartSpeed * randomFloat(emitter.vortexStartSpeedRand.x, emitter.vortexStartSpeedRand.y);
    float endSpeed    = emitter.vortexEndSpeed * randomFloat(emitter.vortexEndSpeedRand.x, emitter.vortexEndSpeedRand.y);
    float startSize   = emitter.vortexStartSize * randomFloat(emitter.vortexStartSizeRand.x, emitter.vortexStartSizeRand.y);
    float endSize     = emitter.vortexEndSize * randomFloat(emitter.vortexEndSizeRand.x, emitter.vortexEndSizeRand.y);
    float randLife    = emitter.vortexMaxParticleLife * randomFloat(emitter.vortexLifeRand.x, emitter.vortexLifeRand.y);
    
    glm::vec3 distance = {randomFloat(emitter.vortexDisX, -emitter.vortexDisY), randomFloat(emitter.vortexDisY, -emitter.vortexDisY), 0.f};

    vortex.pos        = transform.translate + distance;
    vortex.startVel   = {startSpeed * cos(glm::radians(randomAngle)), startSpeed * sin(glm::radians(randomAngle))};
    vortex.endVel     = {endSpeed   * cos(glm::radians(randomAngle)), endSpeed   * sin(glm::radians(randomAngle))};
    vortex.turbulence = emitter.vortexTurbulence;

    vortex.life = vortex.startLife = randLife;
    RL_CORE_TRACE("{}", vortex.life);
    vortex.startSize = vortex.currentSize = startSize;
    vortex.endSize = endSize;
    vortex.timeStep = 0.f;
}

float ParticleSystem::interpolateBetweenRange(float min, float timestep, float max)
{
    return min + (max - min) * timestep;
}

glm::vec4 ParticleSystem::RGBAInterpolation(glm::vec4 startColor, float timestep, glm::vec4 endColor)
{
    glm::vec4 resultColor;

    resultColor = startColor + (endColor - startColor) * timestep;

    return resultColor;
}

}
