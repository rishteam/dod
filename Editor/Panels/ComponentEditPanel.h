#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

namespace rl {

class RL_API ComponentEditPanel
{
public:
    ComponentEditPanel() = default;
    ComponentEditPanel(const Ref<Scene> &scene)
    {
        setContext(scene);
    }
    ~ComponentEditPanel() = default;

    void setContext(const Ref<Scene> &scene)
    {
        m_currentScene = scene;
    }
    void setTarget(const Entity targetEntity)
    {
        m_targetEntity = targetEntity;
    }

    void onImGuiRender();

private:
    Ref<Scene> m_currentScene;
    Entity m_targetEntity;
};

} // end of namespace rl
