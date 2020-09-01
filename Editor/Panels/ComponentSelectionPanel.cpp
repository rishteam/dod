#include "ComponentSelectionPanel.h"

#include <imgui.h>
#include <entt/entt.hpp>

namespace rl {

void ComponentSelectionPanel::onImGuiRender()
{
    // TODO: search widget
    // TODO: child window
    for (auto && [k, v] : m_typeNameToAddFunction)
    {
        if (ImGui::Selectable(k.c_str()+4))
        {
            addComponent(getSelectedEntity(), k);
        }
    }
}

void ComponentSelectionPanel::onAttach(const Ref<Scene> &scene)
{
    SceneTargetPanel::onAttach(scene);
    registerComponent<TransformComponent>();
    registerComponent<RenderComponent>();
}

} // end of namespace rl
