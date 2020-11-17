#pragma once

#include <Rish/rlpch.h>

namespace rl {

class Scene;

class RL_API Animation2DSystem
{
public:
    static void RegisterScene(const Ref<Scene> &scene);
    //
    static void OnUpdate(float dt);
    static void OnImGuiRender();
    //
    static void OnScenePlay();
    static void OnSceneStop();
private:
    static Ref<Scene> s_Scene;
};

}