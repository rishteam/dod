#include "SceneHierarchyPanel.h"

#include <Rish/Utils/String.h>
//
#include <IconsFontAwesome5.h>
#include <Rish/ImGui/ImGui.h>

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
    m_showEntity.clear();
    m_currentScene->m_registry.each([&](auto entityID) {
        Entity entity(entityID, m_currentScene.get());
        if((filterText.empty() || String::isSubString(entity.getName(), filterText)))
        {
            m_showEntity.insert(entity);
        }
    });

    for(auto e : m_showEntity){
        if( m_isFocusEntity && e == m_focusEntity ){
            ImGui::SetScrollHereY(0);
            m_isFocusEntity = false;
        }
        drawEntityNode(e);
    }

    // Draw hide entity hierarchy
    for(auto e : m_hideEntity)
        drawHideEntityNode(e);

    // Reset selected when click empty space in the window
    if(isSelected() &&
       /*ImGui::IsWindowFocused() &&*/ ImGui::IsWindowHovered() &&
       ImGui::IsAnyItemActive())
    {
        resetSelected();
    }

    // Right click menu
    if (ImGui::BeginPopupContextWindow()) {
        if(!isSelected()){
            if (ImGui::MenuItem("Create Entity")) {
                setFocus(m_currentScene->createEntity());
                resetTarget();
                addTarget(m_focusEntity);
            }
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

                auto entSet = getSelectedEntities();
                resetTarget();
                bool first = true;
                for (auto &ent : entSet) {
                    auto copyEntity =  m_currentScene->duplicateEntity(ent);
                    addTarget(copyEntity);
                    if( first ){
                        setFocus(copyEntity);
                    }
                    first = false;
                }

            }
            if (ImGui::MenuItem("Hide Entity"))
            {
            }
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

        if(isSelected() && ImGui::GetIO().KeyShift)
        {
            bool isPassSelect = false;
            bool isPassClick  = false;
            for(auto ent:m_showEntity)
            {
//                Entity ent{entityID, m_currentScene.get()};
                if( m_entitySet.count(ent) && ent != entity )
                    isPassSelect = true;
                if( ent == entity )
                    isPassClick = true;
                if((isPassSelect^isPassClick))
                    addTarget(ent);
            };
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

void SceneHierarchyPanel::drawHideEntityNode(Entity entity){

}

void SceneHierarchyPanel::setFocus(Entity entity){
    m_isFocusEntity = true;
    m_focusEntity = entity;
}

} // end of namespace rl
