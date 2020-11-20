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
    std::set<Entity> &getTargets(){
        removeGroupTarget();
        return m_entitySet;
    }
    void addTarget(const Entity targetEntity)
    {
        m_entitySet.insert(targetEntity);
        removeGroupTarget();
        m_isSelected = !m_entitySet.empty();
    }
    void addTarget(const std::set<Entity> &targetSet)
    {
        m_entitySet.insert(targetSet.begin(), targetSet.end());
        removeGroupTarget();
        m_isSelected = !m_entitySet.empty();
    }

    void removeTarget(const Entity targetEntity)
    {
        m_entitySet.erase(targetEntity);
        if(m_entitySet.empty())
            m_isSelected = false;
    }
    void resetTarget()
    {
        m_isSelected = false;
        m_entitySet.clear();
    }
    void removeGroupTarget()
    {
        for(auto ent : m_entitySet )
        {
            if(!ent.hasComponent<GroupComponent>())
                return;
            auto &gc = ent.getComponent<GroupComponent>();
            for(const auto& id : gc)
            {
                Entity RemoveEntity = m_currentScene->getEntityByUUID(id);
                m_entitySet.erase(RemoveEntity);
            }
        }
    }

    // For selection
    std::set<Entity> &getSelectedEntities()             { removeGroupTarget(); return m_entitySet; }
    bool isSelected() const                             { return m_isSelected; }
    void resetSelected()                                { m_isSelected = false; m_entitySet.clear(); }
    size_t selectedSize() const                         { return m_entitySet.size(); }
protected:
    bool m_isSelected = false;
    std::set<Entity> m_entitySet;
};

} // end of namespace rl


