#pragma once

#include <Rish/rlpch.h>
#include <Rish/ImGui.h>

#include "Panels/Panel.h"

namespace rl {

class RL_API SettingPanel : public Panel
{
public:
    void onImGuiRender() override;
};


}

