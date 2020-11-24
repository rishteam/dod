#pragma once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

#include "SceneMultiTargetPanel.h"

namespace rl {

class RL_API SceneHierarchyPanel : public SceneMultiTargetPanel
{
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &scene) : SceneMultiTargetPanel(scene) {}
    ~SceneHierarchyPanel() override = default;

    void onImGuiRender() override;
    void setFocus(Entity entity);

    void createEntityToTarget();
    void deleteTargetEntities();
    void deleteEntity(Entity delEntity);
    void duplicateTargetEntities();
    Entity duplicateEntity(Entity targetEntity);
    void groupTargetEntities();
    void removeGroupEntity();
    void moveOutGroupEntity();
    void moveIntoGroupEntity(Entity groupEntity);
    bool isCircleGroup(Entity groupEntity, Entity targetEntity);

private:
    void drawEntityNode(Entity entity, bool isSub=false);
    void drawHideEntityNode(Entity entity);

    std::set <Entity> m_showEntity;
    void buildEntitySet(Entity entity);

    std::set <Entity> m_delEntity{};
    std::vector <std::pair<UUID,UUID>> m_groupPair{};

    void resetDrawEntityState();
    std::set<UUID> m_groupEntityUUID{};
    std::set<UUID> m_subEntityUUID{};

    bool m_isFocusEntity = false;
    bool m_isPreFocus = false;
    Entity m_focusEntity;

    const char* m_panelTitle = ICON_FA_LIST " Hierarchy";

};

} // end of namespace rl
