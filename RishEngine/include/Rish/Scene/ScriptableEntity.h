#pragma once

#include <Rish/Scene/Entity.h>
#include <Rish/ImGui.h>

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
        RL_CORE_TRACE("[Debug] ScriptableEntity(): {}", (void*)this);
    }
    virtual ~ScriptableEntity()
    {
        RL_CORE_TRACE("[Debug] ~ScriptableEntity(): {}", (void*)this);
    }

    template<typename T>
    T& getComponent()
    {
        return m_entity.getComponent<T>();
    }

    virtual void onCreate() {}
    virtual void onDestroy() {}
    virtual void onUpdate(Time dt) = 0;
    virtual void onImGuiRender() = 0;

private:
    Entity m_entity;
    //
    friend class Scene;
    friend class ScriptableManager;
    //
    template<typename T>
    friend void CopyComponent(entt::registry &dst, entt::registry &src,
                             std::unordered_map<UUID, entt::entity>& enttMap,
                             const Ref<Scene> &targetScene);
};

class EmptyScript : public ScriptableEntity
{
public:
    virtual void onUpdate(Time dt) {}
    virtual void onImGuiRender() { ImGui::Text("This is a Empty Script\nIt has no function."); }
};

/**
 * @brief Native Script Component
 * @details 如果要改動 NativeScriptComponent 時，請留意 Scene::copySceneTo()
 * 跟 CopyComponent<NativeScriptComponent>()
 */
struct NativeScriptComponent
{
    Ref<ScriptableEntity> instance = nullptr;
    std::string scriptName = "";
    bool valid = false;

    /////////////////////////////////////////
    // Constructor / Destructor
    /////////////////////////////////////////
    NativeScriptComponent()
    {
//        RL_CORE_TRACE("[Debug] NativeScriptComponent()");
        bind<EmptyScript>();
    }

    ~NativeScriptComponent()
    {
        unbind();
//        RL_CORE_TRACE("[Debug] ~NativeScriptComponent()");
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
    }

    void unbind()
    {
        instance = nullptr;
    }
};

} // end of namespace rl