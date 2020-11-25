#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>
#include <Rish/Scene/ComponentManager.h>

#include "SceneTargetPanel.h"

#include "ComponentSelectionPanel.h"

#include <Rish/ImGui/ImGui.h>

namespace rl {

class RL_API ComponentEditPanel : public SceneTargetPanel
{
public:
    ComponentEditPanel() = default;
    ComponentEditPanel(const Ref<Scene> &scene) : SceneTargetPanel(scene) {}
    ~ComponentEditPanel() override = default;

    void onAttach(const Ref<Scene> &scene) override;
    void onImGuiRender() override;

    template<typename T>
    void drawEditComponentWidget() { RL_CORE_ASSERT(false, "Not supported class"); }

    template<typename T>
    bool drawEditComponentRightClickMenu(bool disableDelete=false)
    {
        bool res = false;
        if( !disableDelete && (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))))
        {
            m_targetEntity.removeComponent<T>();
            res = true;
        }

        if(ImGui::BeginPopupContextItem()) // Right-click menu
        {
            if(ImGui::MenuItem("Delete Component", nullptr, false, !disableDelete))
            {
                m_targetEntity.removeComponent<T>();
                res = true;
            }
            ImGui::EndPopup();
        }
        return res;
    }

    void updateGroupTransform(Entity targetEntity);
private:
    ComponentSelectionPanel m_componentSelectionPanel;
};

} // end of namespace rl
