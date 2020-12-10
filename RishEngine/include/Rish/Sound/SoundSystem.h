#pragma once
#ifndef RISHENGINE_SOUNDSYSTEM_H
#define RISHENGINE_SOUNDSYSTEM_H

#include <Rish/Core/Core.h>
#include <Rish/Scene/Scene.h>

namespace rl {

// 2020-12-06 roy4801
// For now because we use SFML for sound support so we actually doesn't need to do anything in SoundSystem

class RL_API SoundSystem
{
public:
    static void RegisterScene(const Ref<Scene> &scene);
    //
    static void OnInit();
    static void onUpdate();

private:
    static Ref<Scene> s_Scene;
};

} // end of namespace rl

#endif // RISHENGINE_SOUNDSYSTEM_H
