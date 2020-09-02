#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>

#include <entt/entt.hpp>

namespace rl {

class RL_API ComponentManager
{
public:
    using AddComponentFunction=std::function<void(Entity)>;
    using DelComponentFunction=std::function<void(Entity)>;

public:
    template<typename T>
    static void registerComponent()
    {
        // TODO: use std::string_view
        auto name = entt::type_info<T>::name();
        std::string type_name{name.begin(), name.end()};
        s_typeNameToAddFunction[type_name] = [](Entity entity) {
            if(!entity.hasComponent<T>())
                entity.addComponent<T>();
        };
        s_typeNameToDelFunction[type_name] = [](Entity entity) {
            if(entity.hasComponent<T>())
                entity.removeComponent<T>();
        };
    }

    static void addComponent(Entity entity, std::string type);
    static void delComponent(Entity entity, std::string type);

    using AddMapping=std::unordered_map<std::string, AddComponentFunction>;
    using DelMapping=std::unordered_map<std::string, DelComponentFunction>;

    static AddMapping& getAddMapping() { return s_typeNameToAddFunction; }
    static DelMapping& getDelMapping() { return s_typeNameToDelFunction; }
private:
    static std::unordered_map<std::string, AddComponentFunction> s_typeNameToAddFunction;
    static std::unordered_map<std::string, DelComponentFunction> s_typeNameToDelFunction;
};

}
