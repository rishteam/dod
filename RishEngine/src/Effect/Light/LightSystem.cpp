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
    auto rigidView = registry.view<TransformComponent, RigidBody2DComponent>();
    auto ambientView = registry.view<TransformComponent, AmbientLightComponent>();

    RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
    for(auto ambient : ambientView)
    {
        auto &transform = registry.get<TransformComponent>(ambient);
        auto &ambientLight = registry.get<AmbientLightComponent>(ambient);

        Renderer2D::DrawQuad(transform.translate, transform.scale, ambientLight.colorMask);
    }

    for(auto entity : view)
    {
        auto &transform = registry.get<TransformComponent>(entity);
        auto &light = registry.get<LightComponent>(entity);

        for(auto rigidEntity : rigidView)
        {
            auto &rigidTransform = registry.get<TransformComponent>(rigidEntity);
            auto &rigidBody = registry.get<RigidBody2DComponent>(rigidEntity);

            auto rotate = [&](glm::vec3 center, glm::vec3 pt, float angle) -> glm::vec3
            {
                auto tmp = pt - center;
                glm::mat4 trans{1.f};
                trans = glm::rotate(trans, glm::radians(angle), glm::vec3(0, 0, 1.f));

                return (glm::vec3)(trans * glm::vec4(tmp, 0)) + center;
            };

            std::vector<glm::vec3> vertices;
            glm::vec3 point = {rigidTransform.translate.x - rigidTransform.scale.x/2, rigidTransform.translate.y - rigidTransform.scale.y/2, rigidTransform.translate.z/2};
            point = rotate(rigidTransform.translate, point, rigidTransform.rotate);
            vertices.push_back(point);

            point = {rigidTransform.translate.x - rigidTransform.scale.x/2, rigidTransform.translate.y + rigidTransform.scale.y/2, rigidTransform.translate.z/2};
            point = rotate(rigidTransform.translate, point, rigidTransform.rotate);
            vertices.push_back(point);

            point = {rigidTransform.translate.x + rigidTransform.scale.x/2, rigidTransform.translate.y + rigidTransform.scale.y/2, rigidTransform.translate.z/2};
            point = rotate(rigidTransform.translate, point, rigidTransform.rotate);
            vertices.push_back(point);

            point = {rigidTransform.translate.x + rigidTransform.scale.x/2, rigidTransform.translate.y - rigidTransform.scale.y/2, rigidTransform.translate.z/2};
            point = rotate(rigidTransform.translate, point, rigidTransform.rotate);
            vertices.push_back(point);

            auto inRange = [&](std::vector<glm::vec3> vertices) -> bool
            {
                for(int i = 0 ; i < vertices.size() ; i++)
                {
                    if(((light.viewPortPos.x - light.viewPortSize.x) <= vertices[i].x && vertices[i].x <= (light.viewPortPos.x + light.viewPortSize.x)) &&
                       ((light.viewPortPos.y - light.viewPortSize.y) <= vertices[i].y && vertices[i].y <= (light.viewPortPos.y + light.viewPortSize.y)))
                    {
                        return true;
                    }
                }
                return false;
            };

            if(inRange(vertices))
            {
                for(int i = 0 ; i < vertices.size() ; i++)
                {
                    glm::vec3 currentVertex  = vertices[i];
                    glm::vec3 nextVertex     = vertices[(i+1)%vertices.size()];
                    glm::vec3 edge           = nextVertex - currentVertex;
                    glm::vec3 normal         = glm::vec3(edge.y, -edge.x, edge.z);
                    glm::vec3 lightToCurrent = currentVertex - transform.translate;

                    if(glm::dot(normal, lightToCurrent) < 0)
                    {
                        Renderer2D::DrawShadow(transform.translate, currentVertex, nextVertex, light.shadowScale, light.shadowColor);
                    }
                }
            }
        }

        RenderCommand::SetBlendMode(true);
        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::One, RenderCommand::BlendFactor::One);
        Renderer2D::DrawPointLight(transform.translate, light.radius, light.strength, light.viewPortPos,
                                   light.viewPortSize, s_viewport, light.color);
        RenderCommand::SetBlendMode(false);
    }
    RenderCommand::SetBlendMode(true);
}

void LightSystem::onViewportResize(const glm::vec2 &viewport)
{
    s_viewport = viewport;
}

}
