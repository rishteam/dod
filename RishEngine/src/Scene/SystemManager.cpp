#include <Rish/rlpch.h>

#include <Rish/Scene/SystemManager.h>
#include <Rish/Effect/Particle/ParticleSystem.h>
#include <Rish/Scene/System/NativeScriptSystem.h>
#include <Rish/Collider/ColliderSystem.h>
#include <Rish/Physics/PhysicsSystem.h>
#include <Rish/Scene/System/SpriteRenderSystem.h>
#include <Rish/Animation/Animation2DSystem.h>
#include <Rish/Sound/SoundSystem.h>

namespace rl
{

void SystemManager::Init(Ref<Scene> &scene)
{
    // Physics
    PhysicsSystem::RegisterScene(scene);
    ColliderSystem::RegisterScene(scene);

    // Script
    NativeScriptSystem::RegisterScene(scene);

    // Render
    SpriteRenderSystem::RegisterScene(scene);
    ParticleSystem::RegisterScene(scene);
    Animation2DSystem::RegisterScene(scene);

    // Sound
    SoundSystem::RegisterScene(scene);
}

void SystemManager::Shutdown()
{

}

} // end of namespace rl