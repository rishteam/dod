#pragma once

#include <Rish/Scene/Scene.h>
#include <Rish/Renderer/Renderer2D.h>
#include <entt/entt.hpp>

namespace rl {

class SpriteRenderSystem
{
public:
    static void RegisterScene(const Ref<Scene> &scene);
    //
    static void OnInit();
    static void onRender();

private:
    static Ref<Scene> s_Scene;
};

} // end of namespace rl