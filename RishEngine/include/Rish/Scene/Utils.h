#pragma once

#include <Rish/Utils/uuid.h>
#include <Rish/Scene/Component.h>
#include <entt/entt.hpp>

namespace rl {

template<typename T>
void CopyComponent(entt::registry &dst, entt::registry &src,
                   const std::unordered_map<UUID, entt::entity>& enttMap)
{
    // Copy all src entities fro src registry to dst registry
    auto view = src.view<T>();
    for(auto srcEnt : view)
    {
        // Get destination entity by id
        auto id = src.get<TagComponent>(srcEnt).id;
        auto dstEnt = enttMap[id];

        auto &srcComponent = src.get<T>(srcEnt);
        auto &dstComponent = dst.emplace_or_replace<T>(dstEnt, srcComponent);
    }
}

} // end of namespace rl