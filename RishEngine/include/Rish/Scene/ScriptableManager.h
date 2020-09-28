#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/ScriptableEntity.h>

namespace rl {

class RL_API ScriptableManager
{
public:
    template<typename T>
    static void Register()
    {
        std::string_view typeName = entt::type_info<T>::name();
        s_scriptBindMap[typeName] = [](NativeScriptComponent &script) {
            script.bind<T>();
        };
    }

    static bool Bind(NativeScriptComponent &script, std::string_view typeName)
    {
        if(!s_scriptBindMap.count(typeName)) {
            RL_CORE_ERROR("NativeScript {} not exist", typeName);
            return false;
        }
        auto &bindFunc = s_scriptBindMap[typeName];
        bindFunc(script);
        return true;
    }
    //
    using BindFunc=void (*)(NativeScriptComponent &);
    using TypeToBindFuncMap = std::unordered_map<std::string_view, BindFunc>;
private:
    static TypeToBindFuncMap s_scriptBindMap;
};

} // end of namespace rl