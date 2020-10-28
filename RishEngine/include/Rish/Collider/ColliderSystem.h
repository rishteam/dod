#pragma once

#include <Rish/rlpch.h>
#include <Rish/Scene/Component.h>
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Scene/Scene.h>

#include <Rish/ImGui.h>
#include <entt/entt.hpp>

namespace rl {

class RL_API ColliderSystem
{
public:
    static void onInit(const Ref <Scene> &scene);

    static void onUpdate(entt::registry& registry, float dt, Scene::SceneState state);


private:
    static Ref<Scene> s_Scene;
};




}