#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Scene.h>
#include <Rish/Renderer/Renderer2D.h>
#include <entt/entt.hpp>

namespace rl {

class NativeScriptSystem
{
public:
    static void RegisterScene(const Ref<Scene> &scene);
    //
    static void OnInit();
    static void OnUpdate(float dt);

    static void onScenePlay();
    static void onSceneStop();

private:
    static Ref<Scene> s_Scene;
};

} // end of namespace rl