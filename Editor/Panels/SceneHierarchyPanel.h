#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

namespace rl {

class RL_API SceneHierarchyPanel
{
public:
    SceneHierarchyPanel();
    SceneHierarchyPanel(const Ref<Scene> &scene);
    ~SceneHierarchyPanel();

    void setScene(const Ref<Scene> &scene);

    void onImGuiRender();

    bool isSelected();

    Entity getSelectedEntity() const { return m_selectedEntity; }
    void resetSelected() { m_isSelected = false; m_selectedEntity = Entity();}

private:
    void drawEntityNode(Entity entity);

    Ref<Scene> m_currentScene;
    Entity m_selectedEntity;
    bool m_isSelected = false;
};

} // end of namespace rl
