#include "SceneHierarchyPanel.h"

#include <imgui.h>

namespace rl {

SceneHierarchyPanel::SceneHierarchyPanel() = default;

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene)
{
    setContext(scene);
}

SceneHierarchyPanel::~SceneHierarchyPanel() = default;

void SceneHierarchyPanel::onImGuiRender()
{
    ImGui::Text("Entity List");

    ImGuiWindowFlags window_flags = 0;
    ImGui::BeginChild("EntityListWindow", ImVec2(0, 300), true, window_flags);
    m_currentScene->m_registry.each([&](auto entityID)
    {
        Entity entity(entityID, m_currentScene.get());
        drawEntityNode(entity);
    });

    if(ImGui::BeginPopupContextWindow())
    {
        if(ImGui::MenuItem("Create Entity"))
        {
            m_currentScene->createEntity();
        }
        ImGui::MenuItem("B");
        ImGui::EndPopup();
    }
    ImGui::EndChild();
}

void SceneHierarchyPanel::setContext(const Ref<Scene> &scene)
{
    m_currentScene = scene;
}

void SceneHierarchyPanel::drawEntityNode(Entity entity)
{
    auto &tag = entity.getComponent<TagComponent>().tag;

    ImGuiTreeNodeFlags nodeFlags = m_selectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0;
    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

    bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, nodeFlags, tag.c_str());
    if(ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;
        m_isSelected = true;
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