/**
 * @file Editor.h
 * @author roy4801 (me@roy4801.tw)
 * @brief Header of Animation Editor
 * @date 2020-05-28
 */
#pragma once

#include <Rish/rlpch.h>

#include <Rish/ImGui/ImGuiWindow.h>
#include <Rish/ImGui/AttributeWidget.h>

namespace rl {

/**
 * @brief Animation Editor class
 * @details Editor for editing Animation
 */
class AnimationEditor : public rl::ImGuiWindow
{
public:
    AnimationEditor();
    virtual ~AnimationEditor() override;

    void onImGuiUpdate() override;

private:
    ImGui::AttributeWidget m_attribWidget;
};

} // end of namespace rl
