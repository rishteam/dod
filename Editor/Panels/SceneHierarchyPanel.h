#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

#include "SceneTargetPanel.h"

namespace rl {

class RL_API SceneHierarchyPanel : public SceneTargetPanel
{
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &scene) : SceneTargetPanel(scene) {}
    ~SceneHierarchyPanel() override = default;

    void onImGuiRender() override;

    size_t selectedSize() const { return m_entitySet.size(); }
private:
    void drawEntityNode(Entity entity);
    std::set<Entity> m_entitySet;
};

} // end of namespace rl
