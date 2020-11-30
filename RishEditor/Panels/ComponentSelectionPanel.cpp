#include "ComponentSelectionPanel.h"

#include <Rish/Utils/String.h>

#include <Rish/Scene/ComponentManager.h>
#include <Rish/Scene/ScriptableEntity.h>

#include <Rish/ImGui/ImGui.h>
#include <entt/entt.hpp>

namespace rl {

void ComponentSelectionPanel::onImGuiRender()
{
    static std::string filterText;
    ImGui::InputText("##ComponentSelection", &filterText);
    if( m_open )
        ImGui::SetKeyboardFocusHere(0);
    m_open = false;

    if(ImGui::ListBoxHeader("##Components"))
    {
        auto &mapping = ComponentManager::getAddMapping();
        for (auto &&[k, v] : mapping) {
            if(filterText.empty() || String::isSubStringIgnoreCase(k, filterText))
            {
                if (ImGui::Selectable(k.c_str() + 4))
                {
                    ComponentManager::AddComponentByTypeName(getSelectedEntity(), k);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
                {
                    printf("%s\n", k.c_str());
                    ComponentManager::AddComponentByTypeName(getSelectedEntity(), k);
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        ImGui::ListBoxFooter();
    }
}

void ComponentSelectionPanel::onAttach(const Ref<Scene> &scene)
{
    SceneTargetPanel::onAttach(scene);
    ComponentManager::RegisterComponent<TransformComponent>();
    ComponentManager::RegisterComponent<SpriteRenderComponent>();
    ComponentManager::RegisterComponent<CameraComponent>();
    ComponentManager::RegisterComponent<NativeScriptComponent>();
    ComponentManager::RegisterComponent<RigidBody2DComponent>();
    ComponentManager::RegisterComponent<BoxCollider2DComponent>();
    ComponentManager::RegisterComponent<Joint2DComponent>();
    ComponentManager::RegisterComponent<ParticleComponent>();
    ComponentManager::RegisterComponent<Animation2DComponent>();
}

} // end of namespace rl
