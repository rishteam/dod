#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/ScriptableEntity.h>

namespace rl {

class Entity;

/**
 * @brief Manager of ScriptableEntity
 * @details
 */
class RL_API ScriptableManager
{
public:
    template<typename T>
    static void Register()
    {
        // Register the type to the list
        std::string typeName = std::string{entt::type_info<T>::name()};
        s_typeName.emplace_back(typeName);

        // Set the binding function
        s_scriptBindMap[typeName] = [](Entity ent)
        {
            auto &script = ent.getComponent<NativeScriptComponent>();
            script.bind<T>();
            script.instance->m_entity = ent;
        };
    }

    static void Shutdown()
    {
    }

    static bool Bind(Entity ent, const std::string &typeName)
    {
        if(!s_scriptBindMap.count(typeName)) {
            RL_CORE_ERROR("NativeScript {} not exist", typeName);
            return false;
        }

        // Bind the script
        auto &bindFunc = s_scriptBindMap[typeName];
        bindFunc(ent);
        return true;
    }

    static void Unbind(Entity ent)
    {
        auto &script = ent.getComponent<NativeScriptComponent>();
        script.unbind();
    }

    template<typename T>
    static std::string GetName()
    {
        return std::string{entt::type_info<T>::name()};
    }
    //
    using BindFunc=void (*)(Entity);
    using TypeToBindFuncMap = std::unordered_map<std::string, BindFunc>;

private:
    static TypeToBindFuncMap s_scriptBindMap;    ///< Type Name Mapping
    static std::vector<std::string> s_typeName;  ///< Type Name list

    static const std::vector<std::string>& GetScriptNames()
    {
        return s_typeName;
    }

    // Friend classes
    friend class ComponentEditPanel;
};

} // end of namespace rl