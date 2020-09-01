#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

#include "SceneTargetPanel.h"

#include "ComponentSelectionPanel.h"

namespace rl {

class RL_API ComponentEditPanel : public SceneTargetPanel
{
public:
    ComponentEditPanel() = default;
    ComponentEditPanel(const Ref<Scene> &scene) : SceneTargetPanel(scene) {}
    ~ComponentEditPanel() override = default;

    void onAttach(const Ref<Scene> &scene) override;
    void onImGuiRender() override;

private:
    ComponentSelectionPanel m_componentSelectionPanel;
};

} // end of namespace rl
