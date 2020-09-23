#pragma once

#include <Rish/Scene/Entity.h>

namespace rl {

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

    using NewFunc    = ScriptableEntity* (*)();
    using DeleteFunc = void (*)(NativeScriptComponent*);

    NewFunc newScript       = nullptr;
    DeleteFunc deleteScript = nullptr;

    template<typename T>
    void bind()
    {
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