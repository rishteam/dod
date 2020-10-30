#pragma once

#include <Rish/rlpch.h>
#include <Rish/Scene/Component.h>
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Scene/Scene.h>

#include <Rish/ImGui.h>
#include <entt/entt.hpp>

namespace rl {

class RL_API PhysicsSystem
{
public:
    static void onUpdate(const Ref<Scene>& scene, float dt);
    static void onInit(const Ref<Scene> &scene);


    static void onImGuiRender();
    static void onUpdateNewPhysicsObject(entt::registry& registry, Scene::SceneState state);
    static void onCleanPhysicObject();
    static void onUpdate(entt::registry& registry, float dt, Scene::SceneState state);
    static void onScenePlay(entt::registry& registry, Scene::SceneState state);
    static void onSceneStop();



private:
    static Ref<Scene> s_Scene;
};


}