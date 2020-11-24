#include "SceneHierarchyPanel.h"

#include <Rish/Utils/String.h>
//
#include <IconsFontAwesome5.h>
#include <Rish/ImGui/ImGui.h>

namespace rl {

void SceneHierarchyPanel::onImGuiRender()
{
    // Begin window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin(m_panelTitle);
    ImGui::PopStyleVar();
    ImGui::Text("Entity List");

    // Search bar
    static std::string filterText;
    ImGui::SetNextItemWidth(-1.f);
    ImGui::InputText("##EntitySelection", &filterText);

    // Entity List Window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar|ImGuiWindowFlags_AlwaysHorizontalScrollbar|ImGuiWindowFlags_AlwaysVerticalScrollbar;
    ImGui::BeginChild("EntityListWindow", ImVec2(0, 0), true, window_flags);

    // Reset
    resetDrawEntityState();
    // Collects all sub entities
    auto view = m_currentScene->m_registry.view<GroupComponent>();
    for(auto ent : view)
    {
        Entity entity{ent, m_currentScene.get()};
        auto &gc = entity.getComponent<GroupComponent>();
        //
        m_groupEntityUUID.insert(entity.getUUID());
        for(const auto &id : gc)
            m_subEntityUUID.insert(id);
    }

    // Draw entity hierarchy
    m_showEntity.clear();
    m_currentScene->m_registry.each([&](auto entityID) {
        Entity entity(entityID, m_currentScene.get());
        if((filterText.empty() || String::isSubStringIgnoreCase(entity.getName(), filterText)))
        {
            if( !m_subEntityUUID.count(entity.getUUID()) )
                buildEntitySet(entity);
        }
    });

    if(!m_isPreFocus && isSelected())
    {
        auto entSet = getTargets();
        setFocus(*entSet.begin());
        m_isPreFocus = false;
    }
    m_isPreFocus = isSelected();

    for(auto e : m_showEntity)
    {
        drawEntityNode(e);
    }


    // Reset selected when click empty space in the window
    if(isSelected() &&
       /*ImGui::IsWindowFocused() &&*/ ImGui::IsWindowHovered() &&
       ImGui::IsAnyItemActive())
    {
        resetSelected();
    }

    // Right click menu
    if (ImGui::BeginPopupContextWindow())
    {
        if(!isSelected())
        {
            if (ImGui::MenuItem("Create Entity"))
            {
                createEntityToTarget();
            }
        }
        if(isSelected())
        {
            auto entSet = getSelectedEntities();
            bool isAllGroupEntity = true;
            bool isAllSubGroupEntity = true;
            for(auto &ent : entSet)
            {
                if(!ent.hasComponent<GroupComponent>())
                    isAllGroupEntity = false;
                if(!ent.hasComponent<SubGroupComponent>())
                    isAllSubGroupEntity = false;
            }

            if (ImGui::MenuItem("Delete Entity"))
            {
                deleteTargetEntities();
            }
            if (ImGui::MenuItem("Duplicate Entity"))
            {
                duplicateTargetEntities();
            }
            if (ImGui::MenuItem("Group Entity"))
            {
                groupTargetEntities();
            }
            if ( isAllGroupEntity )
            {
                if (ImGui::MenuItem("Remove Group"))
                {
                    removeGroupEntity();
                }
            }
            if ( isAllSubGroupEntity )
            {
                if (ImGui::MenuItem("Move out Group"))
                {
                    moveOutGroupEntity();
                }
            }
            if (ImGui::BeginMenu("Move into Group"))
            {
                for(auto &id : m_groupEntityUUID)
                {
                    Entity groupEntity = m_currentScene->getEntityByUUID(id);
                    bool isLegal = true;
                    for(auto &ent : entSet)
                    {
                        // check is entity already under this group
                        if(isCircleGroup(ent, groupEntity)) {
                                isLegal = false;
                            }
                            if(ent.hasComponent<SubGroupComponent>())
                            {
                                auto &sgc = ent.getComponent<SubGroupComponent>();
                                if(sgc.getGroupEntityID() == groupEntity.getUUID())
                                    isLegal = false;
                        }
                    }

                    if( !isSelected(groupEntity) && isLegal )
                    {
                        if( ImGui::MenuItem(groupEntity.getName().c_str()) )
                        {
                            moveIntoGroupEntity(groupEntity);
                        }
                    }
                }
                ImGui::EndMenu();
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

void SceneHierarchyPanel::drawEntityNode(Entity entity, bool isSub)
{
    UUID entUUID = entity.getUUID();

    // Skip the entity when its drawn in sub entities
    if(!isSub && m_subEntityUUID.count(entUUID))
        return;

    auto &tag = entity.getComponent<TagComponent>().tag;

    ImGuiTreeNodeFlags nodeFlags = isSelected(entity) ? ImGuiTreeNodeFlags_Selected : 0;
    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
    nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;
    nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

    bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, nodeFlags, tag.c_str());
    // Set Focus at Hierarchy Window
    if(m_isFocusEntity && entity == m_focusEntity)
    {
        ImGui::SetScrollHereX(0);
        ImGui::SetScrollHereY(0);
        m_isFocusEntity = false;
    }

    if( ImGui::IsItemClicked() ||
       (ImGui::IsItemFocused()&&ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))))
    {
        // If not pressed then clear
        if(!ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift)
        {
            resetTarget();
            m_isPreFocus = false;
        }
        addTarget(entity);

        // Multi selection
        if(isSelected() && ImGui::GetIO().KeyShift)
        {
            bool isPassSelect = false;
            bool isPassClick  = false;
            for(auto ent : m_entitySet)
            {
                if( isSelected(ent) && ent != entity )
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
        // Has sub entity?
        if(entity.hasComponent<GroupComponent>())
        {
            auto &gc = entity.getComponent<GroupComponent>();
            std::set<UUID> removeSet{};
            //
            for(const auto &id : gc)
            {
                // Not valid
                if(!m_currentScene->isValidUUID(id))
                {
                    removeSet.insert(id);
                    continue;
                }
                // Get sub entity and draw
                Entity ent = m_currentScene->getEntityByUUID(id);
                drawEntityNode(ent, true);
            }

            // Cleanup
            for(auto &i : removeSet)
            {
                gc.delEntityUUID(i);
                //
                m_subEntityUUID.erase(i);
            }
        }
        ImGui::TreePop();
    }
}

void SceneHierarchyPanel::createEntityToTarget()
{
    auto newEntity = m_currentScene->createEntity();
    resetTarget();
    addTarget(newEntity);
    setFocus(newEntity);
}

void SceneHierarchyPanel::deleteTargetEntities()
{

    m_delEntity.clear();
    auto entSet = getSelectedEntities();

    for (auto ent : entSet)
    {
        if(ent.hasComponent<SubGroupComponent>())
        {
            auto &sgc = ent.getComponent<SubGroupComponent>();
            Entity preGroupEntity = m_currentScene->getEntityByUUID(sgc.getGroupEntityID());
            auto &gc = preGroupEntity.getComponent<GroupComponent>();
            gc.delEntityUUID(ent.getUUID());
        }
        deleteEntity(ent);
    }

    for(auto ent : m_delEntity)
        ent.destroy();
    m_delEntity.clear();

    resetSelected();
}

void SceneHierarchyPanel::deleteEntity(Entity delEntity)
{
    if(delEntity.hasComponent<GroupComponent>())
    {
        auto &gc = delEntity.getComponent<GroupComponent>();
        for(auto &id : gc)
        {
            Entity subEntity = m_currentScene->getEntityByUUID(id);
            deleteEntity(subEntity);
        }
    }
    m_delEntity.insert(delEntity);
}

void SceneHierarchyPanel::duplicateTargetEntities()
{
    auto entSet = getSelectedEntities();
    resetTarget();
    //
    bool first = true;
    m_groupPair.clear();
    for (auto ent : entSet)
    {
        Entity copyEntity = duplicateEntity(ent);
        if(ent.hasComponent<SubGroupComponent>())
        {
            auto &sgc = ent.getComponent<SubGroupComponent>();
            m_groupPair.push_back({sgc.getGroupEntityID(),copyEntity.getUUID()});
        }
        addTarget(copyEntity);
        //
        if(first)
        {
            setFocus(copyEntity);
            first = false;
        }
    }
    for(auto gp : m_groupPair)
    {
        Entity parentEntity = m_currentScene->getEntityByUUID(gp.first);
        Entity childEntity = m_currentScene->getEntityByUUID(gp.second);

        if(!parentEntity.hasComponent<GroupComponent>())
            parentEntity.addComponent<GroupComponent>();
        auto &gc = parentEntity.getComponent<GroupComponent>();
        auto &sgc = childEntity.addComponent<SubGroupComponent>();

        gc.addEntityUUID(childEntity.getUUID());
        sgc.setGroup(parentEntity.getUUID());
    }

}

Entity SceneHierarchyPanel::duplicateEntity(Entity targetEntity)
{
    Entity copyEntity =  m_currentScene->duplicateEntity(targetEntity);
    if(targetEntity.hasComponent<GroupComponent>())
    {
        auto &targetGc = targetEntity.getComponent<GroupComponent>();
        for(auto &id : targetGc)
        {
            Entity targetSubEntity = m_currentScene->getEntityByUUID(id);
            Entity subEntity = duplicateEntity(targetSubEntity);
            m_groupPair.push_back({copyEntity.getUUID(),subEntity.getUUID()});
        }
    }

    return copyEntity;
}

void SceneHierarchyPanel::groupTargetEntities()
{

    if(!isSelected())
        return;

    Entity group = m_currentScene->createEntity("Group");
    auto &gc = group.addComponent<GroupComponent>();

    bool alreadyInGroup = false;
    for(auto ent : getTargets())
    {
        if(ent.hasComponent<SubGroupComponent>())
            alreadyInGroup = true;
        gc.addEntityUUID(ent.getUUID());
    }

    if(alreadyInGroup)
    {
        group.destroy();
        RL_CORE_ERROR("Entities are already have their own group");
        return;
    }

    for(const auto &id : gc)
    {
        Entity ent = m_currentScene->getEntityByUUID(id);
        auto &sgc = ent.addComponent<SubGroupComponent>();
        sgc.setGroup(group.getUUID());
    }

    resetTarget();
    addTarget(group);

}

void SceneHierarchyPanel::removeGroupEntity()
{
    auto entSet = getSelectedEntities();
    for (auto ent : entSet)
    {
        bool isSub = ent.hasComponent<SubGroupComponent>();
        Entity preGroupEntity;
        if(isSub)
        {
            preGroupEntity = m_currentScene->getEntityByUUID(ent.getComponent<SubGroupComponent>().getGroupEntityID());
            preGroupEntity.getComponent<GroupComponent>().delEntityUUID(ent.getUUID());
        }

        if(ent.hasComponent<GroupComponent>())
        {
            auto &gc = ent.getComponent<GroupComponent>();
            for(auto &id : gc)
            {
                Entity SubEntity = m_currentScene->getEntityByUUID(id);
                if(isSub)
                {
                    preGroupEntity.getComponent<GroupComponent>().addEntityUUID(id);
                    SubEntity.getComponent<SubGroupComponent>().setGroup(preGroupEntity.getUUID());
                }
                else
                {
                    SubEntity.removeComponent<SubGroupComponent>();
                }
            }
        }
        ent.destroy();
    }
    resetSelected();
}

void SceneHierarchyPanel::moveOutGroupEntity()
{
    auto entSet = getSelectedEntities();
    for(auto ent : entSet)
    {
        if(ent.hasComponent<SubGroupComponent>())
        {
            auto &sgc = ent.getComponent<SubGroupComponent>();
            Entity groupEntity = m_currentScene->getEntityByUUID(sgc.getGroupEntityID());
            auto &gc = groupEntity.getComponent<GroupComponent>();
            gc.delEntityUUID(ent.getUUID());
            ent.removeComponent<SubGroupComponent>();
        }
    }
}

void SceneHierarchyPanel::moveIntoGroupEntity(Entity groupEntity)
{
    auto entSet = getSelectedEntities();
    auto &gc = groupEntity.getComponent<GroupComponent>();
    for(auto ent : entSet)
    {
        gc.addEntityUUID(ent.getUUID());
        if(ent.hasComponent<SubGroupComponent>())
        {
            auto &sgc = ent.getComponent<SubGroupComponent>();
            auto preGroupEntity = m_currentScene->getEntityByUUID(sgc.getGroupEntityID());
            auto &preGc = preGroupEntity.getComponent<GroupComponent>();
            preGc.delEntityUUID(ent.getUUID());
        }
        else
        {
            ent.addComponent<SubGroupComponent>();
        }
        auto &sgc = ent.getComponent<SubGroupComponent>();
        sgc.setGroup(groupEntity.getUUID());
    }
}

bool SceneHierarchyPanel::isCircleGroup(Entity groupEntity, Entity targetEntity)
{
    bool isCircle = false;
    if(!groupEntity.hasComponent<GroupComponent>())
        return isCircle;
    auto &gc = groupEntity.getComponent<GroupComponent>();
    for(auto &id : gc)
    {
        Entity ent = m_currentScene->getEntityByUUID(id);
        if( ent.hasComponent<GroupComponent>() )
            isCircle = isCircleGroup(ent, targetEntity);
        if( id == targetEntity.getUUID() )
            isCircle = true;
    }
    return isCircle;
}

///////////////////////////////////////////////////////////////////////////

void SceneHierarchyPanel::drawHideEntityNode(Entity entity)
{

}

void SceneHierarchyPanel::setFocus(Entity entity)
{
    m_isFocusEntity = true;
    m_focusEntity = entity;
}

void SceneHierarchyPanel::resetDrawEntityState()
{
    m_groupEntityUUID.clear();
    m_subEntityUUID.clear();
}

void SceneHierarchyPanel::buildEntitySet(Entity entity)
{
    m_showEntity.insert(entity);
    if( entity.hasComponent<GroupComponent>() )
    {
        auto &gc = entity.getComponent<GroupComponent>();
        for(auto &id : gc )
        {
            Entity ent = m_currentScene->getEntityByUUID(id);
            buildEntitySet(ent);
        }
    }
}

} // end of namespace rl
