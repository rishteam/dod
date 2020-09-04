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
private:
    void drawEntityNode(Entity entity);
};

} // end of namespace rl
