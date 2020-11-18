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
    static void Init();
    static void Shutdown();

    template<typename T>
    static void Register()
    {
        // Register the type to the list
        std::string typeName = std::string{entt::type_info<T>::name()};
        s_typeName.emplace_back(typeName);

        // Set the binding function
        s_scriptBindMap[typeName] = [](Entity ent)
        {
            auto &nativeScriptComponent = ent.getComponent<NativeScriptComponent>();
            nativeScriptComponent.bind<T>(ent);
        };

        // Set the clone function
        s_scriptCloneMap[typeName] = [](Entity ent) -> Ref<ScriptableEntity>
        {
            auto &nativeScriptComponent = ent.getComponent<NativeScriptComponent>();
            return nativeScriptComponent.instance->clone<T>();
        };
    }

    static bool Bind(Entity ent, const std::string &typeName)
    {
        RL_CORE_ASSERT(s_scriptBindMap.count(typeName), "NativeScript {} not exist, Did you called ScriptManager::Register<T>()?", typeName);

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

    static void Copy(Entity dst, Entity src)
    {
        RL_CORE_ASSERT(src.hasComponent<NativeScriptComponent>(), "Source entity doesn't have NSC");
        //
        auto &srcComponent = src.getComponent<NativeScriptComponent>();
        auto &dstComponent = dst.getComponent<NativeScriptComponent>();

        // Copy
        RL_CORE_ASSERT(s_scriptCloneMap.count(srcComponent.scriptName),
                       "NativeScript {} not exist, Did you called ScriptManager::Register<T>()?",
                       srcComponent.scriptName);
        auto &cloneFunc = s_scriptCloneMap[srcComponent.scriptName];
        dstComponent.instance = cloneFunc(src);
        dstComponent.instance->m_entity = dst;
        dstComponent.scriptName = srcComponent.scriptName;
    }

    template<typename T>
    static std::string GetName()
    {
        return std::string{entt::type_info<T>::name()};
    }
    //
    using BindFunc=void (*)(Entity);
    using TypeToBindFuncMap = std::unordered_map<std::string, BindFunc>;

    using CloneFunc = Ref<ScriptableEntity> (*)(Entity);
    using TypeToCloneFuncMap = std::unordered_map<std::string, CloneFunc>;

private:
    static TypeToBindFuncMap s_scriptBindMap;    ///< Type Name Mapping
    static std::vector<std::string> s_typeName;  ///< Type Name list

    static TypeToCloneFuncMap s_scriptCloneMap;  ///< Type Clone Mapping

    static const std::vector<std::string>& GetScriptNames()
    {
        return s_typeName;
    }

    // Friend classes
    friend class ComponentEditPanel;
};

} // end of namespace rl