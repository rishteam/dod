#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Scene.h>
#include <Rish/Renderer/Renderer2D.h>
#include <entt/entt.hpp>

namespace rl {

class SpriteRenderSystem
{
public:
    static void onRuntimeInit(const Ref<Scene> &scene);
    static void onEditorInit(const Ref<Scene> &scene);
    //
    static void onUpdate(entt::registry& registry, float dt, Scene::SceneState state);
    static void onRender(entt::registry& registry, Scene::SceneState state);
    static void onEditorRender(entt::registry& registry, Scene::SceneState state);
    //
    static void onUpdate(const Ref<Scene>& scene, float dt);
    static void onRender(const Ref<Scene>& scene);
    static void onEditorRender(const Ref<Scene>& scene);
};

} // end of namespace rl