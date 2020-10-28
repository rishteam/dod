#pragma once

#include <Rish/Scene/Entity.h>
#include <Rish/ImGui.h>

#include <cereal/types/polymorphic.hpp>

namespace rl {

class NativeScriptComponent;

/**
 * @brief Scriptable Entity
 */
class ScriptableEntity
{
public:
    ScriptableEntity()
    {
    }
    virtual ~ScriptableEntity()
    {
    }

    // Component
    template<typename T>
    T& GetComponent()
    {
        return m_entity.getComponent<T>();
    }

    template<typename T, typename ... Args>
    T& AddComponent(Args &&... args)
    {
        return m_entity.addComponent<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    void RemoveComponent()
    {
        m_entity.removeComponent<T>();
    }

    template<typename T>
    bool HasComponent() const
    {
        return m_entity.hasComponent<T>();
    }

    Entity GetEntity() const { return m_entity; }
    Scene& GetScene() const
    {
        return *m_entity.m_scene;
    }
    Entity GetEntityByHandle(entt::entity ent)
    {
        return Entity{ent, m_entity.m_scene};
    }

    // Main Functions
    virtual void onCreate() {}
    virtual void onDestroy() {}
    virtual void onUpdate(Time dt) = 0;
    virtual void onImGuiRender() = 0;

    // Virtual Copy Constructor pattern
    // https://stackoverflow.com/questions/12255546/c-deep-copying-a-base-class-pointer
    template<class T>
    Ref<T> clone() const
    {
        return Ref<T>(new T(static_cast<T const&>(*this)));
    }

private:
    Entity m_entity;
    //
    friend class Scene;
    friend class ScriptableManager;
    //
    template<typename T>
    friend void CopyComponent(entt::registry &dst, entt::registry &src,
                              std::unordered_map<UUID, entt::entity>& enttMap,
                              Scene *dstScene, Scene *srcScene);
    template<typename T>
    friend void CopyComponentToEntityIfExists(Entity dst, Entity src);
};

// This is a workaround of cereal for empty serialize function
#define RL_SCRIPT_EMPTY_SERIALIZE() \
    template<typename Archive>      \
    void serialize(Archive &ar)     \
    {                               \
        int mock;                   \
        ar(CEREAL_NVP(mock));       \
    }

#define RL_SCRIPT_SERIALIZE()    \
    template<typename Archive>   \
    void serialize(Archive &ar)

#define RL_SERIALIZE(name, object) \
    ar(cereal::make_nvp(name, object))

#define RL_SERIALIZE_NVP(object) \
    ar(CEREAL_NVP(object))

class EmptyScript : public ScriptableEntity
{
public:
    virtual void onUpdate(Time dt) {}
    virtual void onImGuiRender() { ImGui::Text("This is a Empty Script\nIt has no function."); }

    RL_SCRIPT_EMPTY_SERIALIZE()
};

/**
 * @brief Native Script Component
 * @details 如果要改動 NativeScriptComponent 時，請留意 Scene::copySceneTo()
 * 跟 CopyComponent<NativeScriptComponent>() 和 CopyComponentToEntityIfExists<NativeScriptComponent>()
 * `Ref<ScriptableEntity> instance` 會在 Scene::onUpdate() 的一開始被初始化
 */
struct NativeScriptComponent
{
    Ref<ScriptableEntity> instance = nullptr;
    std::string scriptName         = std::string{entt::type_info<EmptyScript>::name()};
    bool valid                     = false;

    /////////////////////////////////////////
    // Constructor / Destructor
    /////////////////////////////////////////
    NativeScriptComponent() = default;
    ~NativeScriptComponent() = default;

    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::make_nvp("instance", instance),
           cereal::make_nvp("script_name", scriptName),
           cereal::make_nvp("valid", valid));
    }

    /////////////////////////////////////////
    // Main Functions
    /////////////////////////////////////////
    template<typename T, typename ... Args>
    void bind(Args&& ... args)
    {
        if(instance)
            instance = nullptr;
        //
        scriptName = entt::type_info<T>::name();
        instance = MakeRef<T>(std::forward<Args>(args)...);
//        RL_CORE_INFO("bind():   {} {} {}", (void*)this, scriptName, (void*)instance.get());
    }

    void unbind()
    {
//        RL_CORE_INFO("unbind(): {} {} {}", (void*)this, scriptName, (void*)instance.get());
        instance = nullptr;
    }
};

} // end of namespace rl

#define RL_REGISTER_SCRIPT_TYPE(x) \
    CEREAL_REGISTER_TYPE(x);       \
    CEREAL_REGISTER_POLYMORPHIC_RELATION(rl::ScriptableEntity, x)

RL_REGISTER_SCRIPT_TYPE(rl::EmptyScript)
