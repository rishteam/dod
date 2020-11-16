#pragma once

#include <Rish/rlpch.h>
#include <Rish/ImGui/ImGui.h>

#include "Panels/Panel.h"

namespace rl {

class HelpPanel : public Panel
{
public:
    void onImGuiRender() override;
};

}