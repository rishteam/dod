/**
 * @file ImGuiLayer.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of ImGuiLayer
 * @date 2020-05-31
 */
#pragma once

#include <imgui.h>

#include <Rish/Core/Core.h>
#include <Rish/Layer/Layer.h>

namespace rl {

/**
 * @brief ImGui Layer
 * @details Handles the ImGui layer things
 */
class RL_API ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
    void onEvent(Event &event) override;

private:
};

}
