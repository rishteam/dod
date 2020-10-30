#include <Rish/Scene/System/NativeScriptSystem.h>

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
        // Is bind
        if(nsc.instance)
        {
            nsc.instance->m_entity = Entity{entityID, s_Scene.get()};
        }
        else
        {
            ScriptableManager::Bind(ent, nsc.scriptName);
        }
    });
}


void NativeScriptSystem::OnUpdate(float dt)
{
    s_Scene->m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        Entity ent{entityID, s_Scene.get()};

        // Is bind
        if(nsc.instance)
        {
            nsc.instance->m_entity = Entity{entityID, s_Scene.get()};
        }
        else
        {
            ScriptableManager::Bind(ent, nsc.scriptName);
        }

        if(nsc.valid)
        {
            nsc.instance->onUpdate(dt);
        }
    });
}

void NativeScriptSystem::onScenePlay()
{
    // Initialize the NativeScriptComponent
    s_Scene->m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
    {
        nsc.instance->onCreate();
        nsc.valid = true;
    });
}

void NativeScriptSystem::onSceneStop()
{
    // Destroy the NativeScriptComponent
    s_Scene->m_registry.view<NativeScriptComponent>().each([=](auto entityID, auto &nsc)
   {
       Entity ent{entityID, s_Scene.get()};
       nsc.instance->onDestroy();
       nsc.valid = false;
   });
}

}