#pragma once

#include <Rish/rlpch.h>

#include "ScenePanel.h"

namespace rl {

class RL_API SceneMultiTargetPanel : public ScenePanel
{
public:
    SceneMultiTargetPanel() = default;
    SceneMultiTargetPanel(const Ref<Scene> &scene) : ScenePanel(scene) {}
    virtual ~SceneMultiTargetPanel() = default;

    virtual void onImGuiRender() = 0;

    // For target
    std::set<Entity>& getTargets()                      { return m_entitySet; }
    const std::set<Entity>& getTargets() const          { return m_entitySet; }
    void addTarget(const Entity targetEntity)           { m_isSelected = true; m_entitySet.insert(targetEntity); }
    void removeTarget(const Entity targetEntity)        { m_entitySet.erase(targetEntity); if(m_entitySet.empty()) m_isSelected = false;}
    void resetTarget()                                  { m_isSelected = false; m_entitySet.clear(); }
    // For selection
    std::set<Entity>& getSelectedEntities()             { return m_entitySet; }
    const std::set<Entity>& getSelectedEntities() const { return m_entitySet; }
    bool isSelected() const                             { return m_isSelected; }
    void resetSelected()                                { m_isSelected = false; m_entitySet.clear(); }
    size_t selectedSize() const                         { return m_entitySet.size(); }
protected:
    bool m_isSelected = false;
    std::set<Entity> m_entitySet;
};

} // end of namespace rl


