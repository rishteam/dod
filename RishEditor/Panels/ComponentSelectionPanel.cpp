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
                    ComponentManager::addComponentByTypeName(getSelectedEntity(), k);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
                {
                    printf("%s\n", k.c_str());
                    ComponentManager::addComponentByTypeName(getSelectedEntity(), k);
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
    ComponentManager::registerComponent<SpriteRenderComponent>();
    ComponentManager::registerComponent<CameraComponent>();
    ComponentManager::registerComponent<NativeScriptComponent>();
    ComponentManager::registerComponent<ParticleComponent>();
    ComponentManager::registerComponent<RigidBody2DComponent>();
    ComponentManager::registerComponent<BoxCollider2DComponent>();
    ComponentManager::registerComponent<Joint2DComponent>();
}

} // end of namespace rl
