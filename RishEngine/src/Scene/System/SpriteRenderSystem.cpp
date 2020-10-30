#include <Rish/Scene/System/SpriteRenderSystem.h>

namespace rl {

Ref<Scene> SpriteRenderSystem::s_Scene;

void SpriteRenderSystem::RegisterScene(const Ref <Scene> &scene)
{
    s_Scene = scene;
}

void SpriteRenderSystem::OnInit()
{
    auto transGroup = s_Scene->m_registry.group<TransformComponent, SpriteRenderComponent>();
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

void SpriteRenderSystem::onRender()
{
    auto transGroup = s_Scene->m_registry.group<TransformComponent, SpriteRenderComponent>();
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

}
