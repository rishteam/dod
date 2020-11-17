#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/ComponentManager.h>

#include "Panels/Common/SceneTargetPanel.h"

namespace rl {

class RL_API ComponentSelectionPanel : public SceneTargetPanel
{
public:
    ComponentSelectionPanel() = default;
    ComponentSelectionPanel(const Ref<Scene> &scene) : SceneTargetPanel(scene) {}
    ~ComponentSelectionPanel() override = default;

    void onAttach(const Ref <Scene> &scene) override;
    void onImGuiRender() override;

    bool m_open = false;

};

} // end of namespace rl


