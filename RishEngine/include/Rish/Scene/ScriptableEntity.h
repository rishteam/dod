#pragma once

#include <Rish/rlpch.h>

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

} // end of namespace rl