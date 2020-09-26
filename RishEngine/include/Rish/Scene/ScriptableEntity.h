#pragma once

#include <Rish/Scene/Entity.h>

namespace rl {

// TODO: Make ScriptableEntity register
class ScriptableEntity
{
public:
    virtual ~ScriptableEntity() = default;

    template<typename T>
    T& getComponent()
    {
        return m_entity.getComponent<T>();
    }

protected:
    virtual void onCreate() {}
    virtual void onDestroy() {}
    virtual void onUpdate(Time dt) {}

private:
    Entity m_entity;
    //
    friend class Scene;
};

/**
 * @brief Native Script Component
 */
struct NativeScriptComponent
{
    ScriptableEntity *instance = nullptr;
    std::string scriptName     = "DefaultScript";

    using NewFunc    = ScriptableEntity* (*)();
    using DeleteFunc = void (*)(NativeScriptComponent*);

    NewFunc newScript       = nullptr;
    DeleteFunc deleteScript = nullptr;

    template<typename T>
    void bind()
    {
        scriptName = entt::type_info<T>::name();
        newScript = []() {
            return static_cast<ScriptableEntity*>(new T());
        };
        deleteScript = [](NativeScriptComponent *nsc) {
            delete nsc->instance;
            nsc->instance = nullptr;
        };
    }
};

} // end of namespace rl