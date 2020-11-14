#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

#include "SceneMultiTargetPanel.h"

namespace rl {

class RL_API SceneHierarchyPanel : public SceneMultiTargetPanel
{
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &scene) : SceneMultiTargetPanel(scene) {}
    ~SceneHierarchyPanel() override = default;

    void onImGuiRender() override;
    void setFocus(Entity entity);

    void createEntity();
    void deleteEntity();
    void duplicateEntity();

private:
    void drawEntityNode(Entity entity);
    void drawHideEntityNode(Entity entity);

    std::set <Entity> m_showEntity;
    std::set <Entity> m_hideEntity;

    bool m_isFocusEntity = false;
    Entity m_focusEntity;
};

} // end of namespace rl
