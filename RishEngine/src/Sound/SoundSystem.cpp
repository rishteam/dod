#include <Rish/rlpch.h>
#include <Rish/Sound/SoundSystem.h>

namespace rl {

Ref<Scene> SoundSystem::s_Scene;

void SoundSystem::RegisterScene(const Ref <Scene> &scene)
{
    s_Scene = scene;
}

void SoundSystem::OnInit()
{
}

void SoundSystem::onUpdate()
{
}

} // end of namespace rl
