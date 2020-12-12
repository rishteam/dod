#pragma once

#include <Rish/rlpch.h>
#include <Rish/ImGui/ImGui.h>
#include <Rish/ImGui/MenuAction.h>

#include "Panels/Panel.h"

namespace rl {

class RL_API SettingPanel : public Panel
{

public:
    void onImGuiRender() override;

private:
    void readData();
    void writeData();
    //
    ImActionManager *m_shortCut = nullptr;
    //
    const char* m_panelTitle = ICON_FA_COG " Setting";
};

}
