#include "SceneHierarchyPanel.h"

#include <IconsFontAwesome5.h>
#include <imgui.h>

namespace rl {

void SceneHierarchyPanel::onImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin(ICON_FA_LIST " Hierarchy");
    ImGui::PopStyleVar();
    ImGui::Text("Entity List");

    // Entity List Window
    ImGuiWindowFlags window_flags = 0;
    ImGui::BeginChild("EntityListWindow", ImVec2(0, 0), true, window_flags);

    // Draw entity hierarchy
    m_currentScene->m_registry.each([&](auto entityID) {
        Entity entity(entityID, m_currentScene.get());
        drawEntityNode(entity);
    });

    // Reset selected when click empty space in the window
    if(isSelected() &&
        ImGui::IsWindowFocused() && ImGui::IsWindowHovered() &&
        ImGui::IsAnyItemActive())
    {
        resetSelected();
    }

    // Right click menu
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Create Entity")) {
            m_currentScene->createEntity();
        }
        if (isSelected() && ImGui::MenuItem("Delete Entity")) {
            auto ent = getSelectedEntity();
            ent.destroy();
            resetSelected();
        }
        ImGui::EndPopup();
    }
    ImGui::EndChild();
    ImGui::End();
}

void SceneHierarchyPanel::drawEntityNode(Entity entity)
{
    auto &tag = entity.getComponent<TagComponent>().tag;

    ImGuiTreeNodeFlags nodeFlags = getTarget() == entity ? ImGuiTreeNodeFlags_Selected : 0;
    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

    bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, nodeFlags, tag.c_str());
    if(ImGui::IsItemClicked())
    {
        setTarget(entity);
    }

    if(opened)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
        if (opened)
            ImGui::TreePop();
        ImGui::TreePop();
    }
}

} // end of namespace rl