#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

#include "ScenePanel.h"

namespace rl {

class RL_API SceneHierarchyPanel : public ScenePanel
{
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &scene) : ScenePanel(scene) {}
    ~SceneHierarchyPanel() override = default;

    void onImGuiRender() override;

    Entity getSelectedEntity() const { return m_selectedEntity; }
    bool isSelected() const { return m_isSelected; }
    void resetSelected() { m_isSelected = false; m_selectedEntity = Entity();}

private:
    void drawEntityNode(Entity entity);
    Entity m_selectedEntity;
    bool m_isSelected = false;
};

} // end of namespace rl
