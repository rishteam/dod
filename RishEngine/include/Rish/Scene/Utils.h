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
                   Scene *dstScene, Scene *srcScene)
{
    RL_UNUSED(srcScene);
    RL_UNUSED(dstScene);

    // Copy all src entities for src registry to dst registry
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
                                          Scene *dstScene, Scene *srcScene)
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
        Entity dstEntity{dstEnt, dstScene};
        Entity srcEntity{srcEnt, srcScene};

        RL_CORE_ASSERT(srcEntity, "Invalid source entity");
        RL_CORE_ASSERT(dstEntity, "Invalid destination entity");

        // Copy the values of instance
        ScriptableManager::Copy(dstEntity, srcEntity);
        RL_CORE_TRACE("CopyComponent<NativeScriptComponent>: dst={} src={}", (void *)dstComponent.instance.get(), (void *)srcComponent.instance.get());
    }
}

template<typename T>
void CopyComponentToEntityIfExists(Entity dst, Entity src)
{
    if(src.hasComponent<T>())
    {
        auto &dstComponent = dst.addComponent<T>();
        dstComponent = src.getComponent<T>();
    }
}

template<>
void CopyComponentToEntityIfExists<NativeScriptComponent>(Entity dst, Entity src)
{
    if(src.hasComponent<NativeScriptComponent>())
    {
        auto &srcComponent = src.getComponent<NativeScriptComponent>();
        auto &dstComponent = dst.addComponent<NativeScriptComponent>();

        // Copy
        ScriptableManager::Copy(dst, src);
    }
}

} // end of namespace rl