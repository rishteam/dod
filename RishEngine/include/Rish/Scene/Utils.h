#pragma once

#include <Rish/Utils/uuid.h>

#include <Rish/Scene/Scene.h>
#include <Rish/Scene/Component.h>
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>

#include <entt/entt.hpp>

namespace rl {

template<typename T>
void CopyComponent(entt::registry &dst, entt::registry &src,
                   std::unordered_map<UUID, entt::entity>& enttMap,
                   const Ref<Scene> &scene)
{
    RL_UNUSED(scene);
    // Copy all src entities fro src registry to dst registry
    auto view = src.view<T>();
    for(auto srcEnt : view)
    {
        // Get destination entity by id
        auto id = src.get<TagComponent>(srcEnt).id;
        auto dstEnt = enttMap[id];

        // Copy the component to destination entity
        auto &srcComponent = src.get<T>(srcEnt);
        dst.emplace_or_replace<T>(dstEnt, srcComponent);
    }
}

template<>
void CopyComponent<NativeScriptComponent>(entt::registry &dst, entt::registry &src,
                   std::unordered_map<UUID, entt::entity>& enttMap,
                   const Ref<Scene> &targetScene)
{
    // Copy all src entities fro src registry to dst registry
    auto view = src.view<NativeScriptComponent>();
    for(auto srcEnt : view)
    {
        // Get destination entity by id
        auto id = src.get<TagComponent>(srcEnt).id;
        auto dstEnt = enttMap[id];

        // Get the source NativeScriptComponent
        auto &srcComponent = src.get<NativeScriptComponent>(srcEnt);
        // Create a new NativeScriptComponent for dst
        auto &dstComponent = dst.emplace<NativeScriptComponent>(dstEnt);

        // RishEngine Entity of dst
        Entity dstEntity{dstEnt, targetScene.get()};

        // Bind the Script to dstEntity
        ScriptableManager::Bind(dstEntity, srcComponent.scriptName);

        // Copy the values of instance
        *dstComponent.instance = *srcComponent.instance;

        // Set the instance->m_entity (Written by the copy above)
        dstComponent.instance->m_entity = dstEntity;

    }
}

} // end of namespace rl