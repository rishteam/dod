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
#include <Rish/Animation/Component.h>


namespace rl {

// fwd
class Animation2DComponent;

/**
 * @brief Animation Editor class
 * @details Editor for editing Animation
 */
class AnimationEditor : public ImGuiWindow
{
public:
    AnimationEditor();
    virtual ~AnimationEditor() override;

    void onImGuiUpdate() override;

    void setTargetComponent(Animation2DComponent *target);
    void resetTargetComponent();
private:

    ImGui::AttributeWidget m_attribWidget;
    Animation2DComponent *m_editTarget = nullptr;
};

} // end of namespace rl
