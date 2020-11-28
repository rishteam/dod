#pragma once

#include <Rish/rlpch.h>
#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Component.h>
#include <Rish/Renderer/RendererCommand.h>
#include <Rish/Renderer/Renderer2D.h>

namespace rl {

class RL_API LightSystem
{
public:
    LightSystem();

    static void RegisterScene(const Ref <Scene> &scene);

    static void OnUpdate(float dt);

    static void onRender();

    static void onEditorRender();

    static void onViewportResize(const glm::vec2 &viewport);

private:

    static Ref<Scene> s_Scene;
    static glm::vec2 s_viewport;
};

}

