#pragma once

#include <Rish/Utils/uuid.h>
#include <Rish/Scene/Component.h>
#include <entt/entt.hpp>

namespace rl {

template<typename T>
void CopyComponent(entt::registry &dst, entt::registry &src,
                   std::unordered_map<UUID, entt::entity>& enttMap)
{
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

} // end of namespace rl