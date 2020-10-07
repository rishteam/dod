#pragma once

#include <Rish/Scene/Entity.h>

namespace rl {

/**
 * @brief Scriptable Entity
 */
class ScriptableEntity
{
public:
    virtual ~ScriptableEntity() = default;

    template<typename T>
    T& getComponent()
    {
        return m_entity.getComponent<T>();
    }

    virtual void onCreate() {}
    virtual void onDestroy() {}
    virtual void onUpdate(Time dt) = 0;
    virtual void onImGuiRender() = 0;

    /**
     * @brief Is the ScriptableEntity has a valid rl::Entity
     * @return
     */
    bool haveValidEntity() const
    {
        return (bool)m_entity;
    }

private:
    Entity m_entity;
    //
    friend class Scene;
};

class EmptyScript : public ScriptableEntity
{
public:
    virtual void onUpdate(Time dt) {}
    virtual void onImGuiRender() {}
};

/**
 * @brief Native Script Component
 */
struct NativeScriptComponent
{
    ScriptableEntity *instance = nullptr;
    std::string scriptName     = "";
    bool valid = false;

    template<typename T, typename ... Args>
    void bind(Args&& ... args)
    {
        scriptName = entt::type_info<T>::name();
        instance = new T(std::forward<Args>(args)...);
    }
//
    void unbind()
    {
        scriptName = "DefaultScript";
        delete instance;
        instance = nullptr;
    }
};

} // end of namespace rl