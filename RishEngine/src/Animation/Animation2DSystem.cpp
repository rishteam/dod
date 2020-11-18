#include <Rish/Animation/Animation2DSystem.h>
//
#include <Rish/Animation/Component.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>
#include <Rish/Renderer/Renderer2D.h>

namespace rl {

Ref<Scene> Animation2DSystem::s_Scene;

void Animation2DSystem::RegisterScene(const Ref <Scene> &scene)
{
    s_Scene = scene;
}

void Animation2DSystem::OnUpdate(float dt)
{
    auto &registry = s_Scene->m_registry;
    auto view = registry.view<Animation2DComponent>();
    for(auto ent : view)
    {
        Entity entity{ent, s_Scene.get()};
        auto &ani = entity.getComponent<Animation2DComponent>();
        auto &trans = entity.getComponent<TransformComponent>();
        //
        if(ani.isTimerUp())
        {
            ani.playNextFrame();
            ani.resetTimer();
        }
    }
}

void Animation2DSystem::OnRender()
{
    auto &registry = s_Scene->m_registry;
    auto view = registry.view<Animation2DComponent>();
    for(auto ent : view)
    {
        Entity entity{ent, s_Scene.get()};
        auto &ani = entity.getComponent<Animation2DComponent>();
        auto &trans = entity.getComponent<TransformComponent>();
        //
        if(trans.rotate != 0.f)
            Renderer2D::DrawQuad(trans.translate, trans.scale, ani.getCurrentFrame());
        else
            Renderer2D::DrawRotatedQuad(trans.translate, trans.scale, ani.getCurrentFrame(), trans.rotate);
    }
}

//////////////////////////////////////////////////////////
// OnEditor
//////////////////////////////////////////////////////////

void Animation2DSystem::OnEditorUpdate(float dt)
{

}

void Animation2DSystem::OnEditorRender()
{

}

//////////////////////////////////////////////////////////

void Animation2DSystem::OnImGuiRender()
{

}

//////////////////////////////////////////////////////////

void Animation2DSystem::OnScenePlay()
{

}

void Animation2DSystem::OnSceneStop()
{

}

} // end of namespace rl
