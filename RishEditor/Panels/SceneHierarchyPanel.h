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

private:
    void drawEntityNode(Entity entity);
};

} // end of namespace rl
