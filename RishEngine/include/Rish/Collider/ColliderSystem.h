#pragma once

#include <Rish/rlpch.h>
#include <Rish/Scene/Component.h>
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Scene/Scene.h>

#include <Rish/ImGui.h>
#include <entt/entt.hpp>

namespace rl {

// TODO: Detected collision for collider Use QuadTree
class RL_API ColliderSystem
{
public:
    static void RegisterScene(const Ref <Scene> &scene);

    static void OnEditorUpdate(std::set<Entity> &selectedEntites);

private:
    static Ref<Scene> s_Scene;
};

}