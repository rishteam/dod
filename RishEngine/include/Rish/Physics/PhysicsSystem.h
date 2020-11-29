#pragma once

#include <Rish/rlpch.h>
#include <Rish/Scene/Component.h>
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Scene/Scene.h>


#include <Rish/ImGui.h>
#include <entt/entt.hpp>

class Event;
class Box;
class Circle;
class Polygon;

namespace rl {

class RL_API PhysicsSystem
{
public:
    static void RegisterScene(const Ref<Scene> &scene);
    //
    static void OnUpdate(float dt);
    static void OnEditorUpdate(std::set<Entity> &selectedEntites);
    static void OnImGuiRender();
    //
    static void OnScenePlay();
    static void OnSceneStop();
private:
    static void PhysicsWorldUpdate(float dt);
    static void InitPhysicsObject(entt::registry& registry, Scene::SceneState state);
    static void UpdateNewPhysicsObject(entt::registry& registry, Scene::SceneState state);
    //
    static Ref<Scene> s_Scene;
};

}
