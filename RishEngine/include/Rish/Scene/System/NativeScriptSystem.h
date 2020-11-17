#pragma once

#include <Rish/rlpch.h>

#include <entt/entt.hpp>

namespace rl {

class Entity;
class Scene;

class NativeScriptSystem
{
public:
    static void RegisterScene(const Ref<Scene> &scene);
    //
    static void OnInit();
    static void OnUpdate(float dt);

    static void OnScenePlay();
    static void OnSceneStop();

private:
    static void InitNativeScript(Entity entity);

    /**
     * @brief On Create
     * @param entity
     */
    static void OnCreate(Entity entity);

    /**
     * @brief On Destroy
     * @details Called at Scene::destroyEntity and NativeScriptSystem::OnSceneStop
     * @param nsc NativeScriptComponent
     */
    static void OnDestroy(Entity entity);

    friend class Scene;

    static Ref<Scene> s_Scene;
};

} // end of namespace rl