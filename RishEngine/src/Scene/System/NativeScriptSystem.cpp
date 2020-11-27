#include <Rish/Scene/System/NativeScriptSystem.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>

namespace rl {

Ref<Scene> NativeScriptSystem::s_Scene;

void NativeScriptSystem::RegisterScene(const Ref<Scene> &scene)
{
    s_Scene = scene;
}

void NativeScriptSystem::OnInit()
{
    s_Scene->m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc) {
        Entity ent{entityID, s_Scene.get()};
        //
        InitNativeScript(ent);
    });
}

void NativeScriptSystem::OnUpdate(float dt)
{
    s_Scene->m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        Entity ent{entityID, s_Scene.get()};
        //
        InitNativeScript(ent);
        //
        if(s_Scene->getSceneState() == Scene::SceneState::Play)
        {
            // Is valid
            if (nsc.valid)
            {
                nsc.instance->onUpdate(dt);
            }
            // not valid
            // OnCreate() entities that add NSC after Scene start
            else
            {
                OnCreate(ent);
            }
        }
    });
}

void NativeScriptSystem::OnScenePlay()
{
    // Initialize the NativeScriptComponent
    s_Scene->m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        Entity ent{entityID, s_Scene.get()};
        //
        OnCreate(ent);
    });
}

void NativeScriptSystem::OnSceneStop()
{
    // Destroy the NativeScriptComponent
    s_Scene->m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        Entity ent{entityID, s_Scene.get()};
        //
        OnDestroy(ent);
    });
}

void NativeScriptSystem::InitNativeScript(Entity ent)
{
    RL_CORE_ASSERT(ent, "Invalid entity");
    //
    auto &nsc = ent.getComponent<NativeScriptComponent>();
    // Is bind
    if(!nsc.instance)
    {
        ScriptableManager::Bind(ent, nsc.scriptName);
    }
    else
    {
        nsc.instance->m_entity = ent;
    }
}

void NativeScriptSystem::OnCreate(Entity entity)
{
    RL_CORE_ASSERT(entity, "Invalid entity");
    //
    if(entity.hasComponent<NativeScriptComponent>())
    {
        auto &nsc = entity.getComponent<NativeScriptComponent>();
        //
        nsc.instance->onCreate();
        nsc.valid = true;
    }
}

void NativeScriptSystem::OnDestroy(Entity entity)
{
    RL_CORE_ASSERT(entity, "Invalid entity");
    //
    if(entity.hasComponent<NativeScriptComponent>())
    {
        auto &nsc = entity.getComponent<NativeScriptComponent>();
        //
        nsc.instance->onDestroy();
        nsc.valid = false;
    }
}

}