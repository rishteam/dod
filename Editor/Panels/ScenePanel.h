#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

namespace rl {

class RL_API ScenePanel
{
public:
    ScenePanel() = default;
    ScenePanel(const Ref<Scene> &scene)
        : m_currentScene(scene)
    {
    }
    virtual ~ScenePanel() = default;

    virtual void onAttach(const Ref <Scene> &scene) { setContext(scene); }
    virtual void onDetach() {}
    virtual void onImGuiRender() = 0;

    void setContext(const Ref<Scene> &scene) { m_currentScene = scene; }
    Ref<Scene> getContext() { return m_currentScene; }
protected:
    Ref<Scene> m_currentScene;
};

} // end of namespace rl


