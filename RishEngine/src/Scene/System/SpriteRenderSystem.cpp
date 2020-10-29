#include <Rish/Scene/System/SpriteRenderSystem.h>

namespace rl {

void SpriteRenderSystem::onRuntimeInit(const Ref<Scene> &scene)
{
    auto transGroup = scene->m_registry.group<TransformComponent, SpriteRenderComponent>();
    for (auto entity: transGroup)
    {
        auto &transform = transGroup.get<TransformComponent>(entity);
        auto &render = transGroup.get<SpriteRenderComponent>(entity);

        if (render.init)
        {
            render.loadFromPath();
            render.init = false;
        }
    }
}

void SpriteRenderSystem::onEditorInit(const Ref<Scene> &scene)
{
}

void SpriteRenderSystem::onUpdate(entt::registry &registry, float dt, Scene::SceneState state)
{
    if(state != Scene::SceneState::Play)
        return;
}

void SpriteRenderSystem::onRender(entt::registry &registry, Scene::SceneState state)
{
    auto transGroup = registry.group<TransformComponent, SpriteRenderComponent>();
    for (auto entity: transGroup)
    {
        auto &transform = transGroup.get<TransformComponent>(entity);
        auto &render = transGroup.get<SpriteRenderComponent>(entity);

        if (render.init)
        {
            render.loadFromPath();
            render.init = false;
        }

        if (render.useTexture)
        {
            if (render.useAsSubTexture)
            {
                if (transform.rotate != 0.f)
                    Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.m_subtexture,
                                                render.color, transform.rotate, render.tiling);
                else
                    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_subtexture,
                                         render.color, render.tiling);
            }
            else // normal texture
            {
                if (transform.rotate != 0.f)
                    Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.m_texture,
                                                render.color, transform.rotate, render.tiling);
                else
                    Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture,
                                         render.color, render.tiling);
            }
        }
        else
        {
            if (transform.rotate != 0.f)
                Renderer2D::DrawRotatedQuad(transform.translate, glm::vec2(transform.scale), render.color,
                                            transform.rotate);
            else
                Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);
        }
    }
}

void SpriteRenderSystem::onEditorRender(entt::registry &registry, Scene::SceneState state)
{

}

void SpriteRenderSystem::onUpdate(const Ref<Scene> &scene, float dt)
{
    auto &registry = scene->m_registry;
    const auto &state = scene->getSceneState();
    //
    onUpdate(registry, dt, state);
}

void SpriteRenderSystem::onRender(const Ref<Scene> &scene)
{
    auto &registry = scene->m_registry;
    const auto &state = scene->getSceneState();
    //
    onRender(registry, state);
}

void SpriteRenderSystem::onEditorRender(const Ref<Scene> &scene)
{
    auto &registry = scene->m_registry;
    const auto &state = scene->getSceneState();
    //
    onEditorRender(registry, state);
}

}
