#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/ScriptableEntity.h>
//
#include <Rish/Renderer/Renderer2D.h>
#include <entt/entt.hpp>

namespace rl {

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