#pragma once

#include <Rish/rlpch.h>

#include "ScenePanel.h"

namespace rl {

class RL_API SceneTargetPanel : public ScenePanel
{
public:
    SceneTargetPanel() = default;
    SceneTargetPanel(const Ref<Scene> &scene) : ScenePanel(scene) {}
    virtual ~SceneTargetPanel() = default;

    virtual void onImGuiRender() = 0;

    // For target
    Entity getTarget() const                  { return m_targetEntity; }
    void setTarget(const Entity targetEntity) { m_isSelected = true; m_targetEntity = targetEntity; }
    void resetTarget()                        { m_isSelected = false; m_targetEntity = Entity(); }
    // For selection
    Entity getSelectedEntity() const          { return m_targetEntity; }
    bool isSelected() const                   { return m_isSelected; }
    void resetSelected()                      { m_isSelected = false; m_targetEntity = Entity(); }
protected:
    Entity m_targetEntity;
    bool m_isSelected = false;
};

} // end of namespace rl


