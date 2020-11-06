#include <Rish/Effect/Light/LightSystem.h>

namespace rl {

Ref<Scene> LightSystem::s_Scene;
glm::vec2 LightSystem::s_viewport;

void LightSystem::RegisterScene(const rl::Ref<rl::Scene> &scene)
{
    s_Scene = scene;
}


void LightSystem::OnUpdate(float dt)
{

}

void LightSystem::onRender()
{
    auto &registry = s_Scene->m_registry;
    auto state     = s_Scene->getSceneState();

    auto view = registry.view<TransformComponent, LightComponent>();

    for(auto entity : view) {
        auto &transform = registry.get<TransformComponent>(entity);
        auto &light = registry.get<LightComponent>(entity);

        Renderer2D::DrawPointLight(transform.translate, light.radius, light.strength, light.viewPortPos,
                                   light.viewPortSize, s_viewport, light.color);
    }
}

void LightSystem::onViewportResize(const glm::vec2 &viewport)
{
    s_viewport = viewport;
}

}
