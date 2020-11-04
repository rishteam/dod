#include "SceneHierarchyPanel.h"

#include <Rish/Utils/String.h>
//
#include <IconsFontAwesome5.h>
#include <Rish/ImGui.h>

namespace rl {

void SceneHierarchyPanel::onImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin(ICON_FA_LIST " Hierarchy");
    ImGui::PopStyleVar();
    ImGui::Text("Entity List");

    //
    static std::string filterText;
    ImGui::SetNextItemWidth(-1.f);
    ImGui::InputText("##EntitySelection", &filterText);

    // Entity List Window
    ImGuiWindowFlags window_flags = 0;
    ImGui::BeginChild("EntityListWindow", ImVec2(0, 0), true, window_flags);

    // Draw entity hierarchy
    m_currentScene->m_registry.each([&](auto entityID) {
        Entity entity(entityID, m_currentScene.get());
        if((filterText.empty() || String::isSubString(entity.getName(), filterText)) &&
            !m_isHidingEntity[entity])
        {
            drawEntityNode(entity);
        }
    });

    // Reset selected when click empty space in the window
    if(isSelected() &&
       /*ImGui::IsWindowFocused() &&*/ ImGui::IsWindowHovered() &&
       ImGui::IsAnyItemActive())
    {
        resetSelected();
    }

    // Right click menu
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Create Entity")) {
            m_currentScene->createEntity();
        }
        if(isSelected())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                for (auto e : m_entitySet)
                    e.destroy();
                resetSelected();
            }
            if (ImGui::MenuItem("Duplicate Entity"))
            {
                for (auto &ent : getSelectedEntities())
                    m_currentScene->duplicateEntity(ent);
                resetSelected();
            }
            if (ImGui::MenuItem("Hide Entity"))
            {
                for (auto e : m_entitySet)
                    m_isHidingEntity[e] = true;
                resetSelected();
            }
        }
        if(ImGui::MenuItem("Show All Entity"))
        {
            m_isHidingEntity.clear();
        }
        ImGui::EndPopup();
    }
    ImGui::EndChild();
    ImGui::End();
}

void SceneHierarchyPanel::drawEntityNode(Entity entity)
{
    auto &tag = entity.getComponent<TagComponent>().tag;

    ImGuiTreeNodeFlags nodeFlags = m_entitySet.count(entity) ? ImGuiTreeNodeFlags_Selected : 0;
    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
    nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, nodeFlags, tag.c_str());
    if(ImGui::IsItemClicked())
    {
        // If not pressed then clear
        if(!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift){
            resetTarget();
        }
        addTarget(entity);

        if(ImGui::GetIO().KeyShift)
        {
            bool select = false;
            m_currentScene->m_registry.each([&](auto entityID)
            {
                Entity ent{entityID, m_currentScene.get()};
                std::cout << ent.getName() << (select?" Y ":" N ");
                if( m_entitySet.count(ent) && ent != entity )
                    select = !select;
                std::cout << (select?" Y ":" N ");
                if( select && !m_isHidingEntity[ent])
                    addTarget(ent);
                if( ent == entity )
                    select = !select;
                std::cout << (select?" Y\n":" N\n");
            });
        }

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