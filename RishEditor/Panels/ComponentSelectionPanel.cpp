#include "ComponentSelectionPanel.h"

#include <Rish/Scene/ComponentManager.h>
#include <Rish/Scene/ScriptableEntity.h>

#include <Rish/ImGui.h>
#include <entt/entt.hpp>

namespace rl {

void ComponentSelectionPanel::onImGuiRender()
{
    // TODO: search widget
    // TODO: child window
    for (auto && [k, v] : ComponentManager::getAddMapping())
    {
        if (ImGui::Selectable(k.c_str()+4))
        {
            ComponentManager::addComponent(getSelectedEntity(), k);
        }
    }
}

void ComponentSelectionPanel::onAttach(const Ref<Scene> &scene)
{
    SceneTargetPanel::onAttach(scene);
    ComponentManager::registerComponent<TransformComponent>();
    ComponentManager::registerComponent<RenderComponent>();
    ComponentManager::registerComponent<CameraComponent>();
    ComponentManager::registerComponent<NativeScriptComponent>();
}

} // end of namespace rl
