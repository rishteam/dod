#include <Rish/Script/Cinemachine2D.h>

namespace rl {

void Cinemachine2D::onCreate()
{
}

void Cinemachine2D::onDestroy()
{
}

void Cinemachine2D::onUpdate(Time dt)
{
    auto target = GetScene().getEntityByUUID(chaseTarget);
    if(target)
    {
        auto &trans = target.getComponent<TransformComponent>();

        // Camera follows player
        auto entity = GetEntity();
        auto &camera = entity.getComponent<CameraComponent>();
        auto &cameraTrans = entity.getComponent<TransformComponent>();

        cameraTrans.translate.x = trans.translate.x;
        //
        float distance = trans.translate.y - cameraTrans.translate.y;
        m_distance = distance;
        if (abs(distance) >= 0.7f)
        {
            // TODO: more smooth camera
            cameraTrans.translate.y += distance * cameraTransSpeed * dt.asSeconds();
        }
    }
}

void Cinemachine2D::onImGuiRender()
{
    ImGui::Text("distance = %.2f", m_distance);
    ImGui::DragFloat("Camera Translate Speed", &cameraTransSpeed, 0.1f);
    ImGui::DragFloat("Start Translate Speed", &startTransSpeed, 0.1f);
    ImGui::DragFloat("End Translate Speed", &endTransSpeed, 0.1f);

    if(ImGui::BeginCombo("Target", chaseTargetName.c_str()))
    {
        auto view = GetScene().m_registry.view<TagComponent>();
        for(auto ent : view)
        {
            auto entity = GetEntityByHandle(ent);
            auto &tag = entity.getComponent<TagComponent>();
            //
            if (ImGui::Selectable(tag.tag.c_str(), chaseTarget == tag.id))
            {
                chaseTarget = tag.id;
                chaseTargetName = tag.tag;
            }
        }
        ImGui::EndCombo();
    }
}

}