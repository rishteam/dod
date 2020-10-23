#pragma once

#include <Rish/rlpch.h>
#include <Rish/ImGui.h>
//
#include <Rish/Renderer/Texture2D.h>

#include "Panels/Panel.h"

namespace rl {

class AboutPanel : public Panel
{
public:
    void onAttach() override;
    void onImGuiRender() override;

private:
    Ref<Texture2D> m_icon;
};

}