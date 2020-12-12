#include "ParkourCameraController.h"

namespace rl{

void ParkourCameraController::onCreate()
{
}

void ParkourCameraController::onDestroy()
{

}

void ParkourCameraController::onUpdate(Time dt)
{

    if( m_followEntityID.to_string() == "None" || !HasComponent<CameraComponent>())
        return;

    auto &scene = GetScene();

    auto ent = scene.getEntityByUUID(m_followEntityID);

    auto &cameraTrans = GetComponent<TransformComponent>();
    auto &followTrans = ent.getComponent<TransformComponent>();

    auto &camera = GetComponent<CameraComponent>();

    glm::vec2 frame(camera.camera.getAspect()*m_frameSize,m_frameSize);

    if(followTrans.translate.x > cameraTrans.translate.x+frame.x/2)
        cameraTrans.translate.x = followTrans.translate.x-frame.x/2;
    if(followTrans.translate.x < cameraTrans.translate.x-frame.x/2)
        cameraTrans.translate.x = followTrans.translate.x+frame.x/2;

    if(followTrans.translate.y > cameraTrans.translate.y)
        cameraTrans.translate.y = followTrans.translate.y;
    if(followTrans.translate.y < cameraTrans.translate.y-frame.y/2)
        cameraTrans.translate.y = followTrans.translate.y+frame.y/2;


//    cameraTrans.translate = followTrans.translate;

}

void ParkourCameraController::onImGuiRender()
{

    if(!HasComponent<CameraComponent>()){
        ImGui::Text("Need Component: CameraComponent");
        return;
    }

    if( m_followEntityID.to_string() == "None" )
        ImGui::Text("Don't have followed Entity");
    else{
        std::string str = "Now Follow is " + m_followEntityID.to_string();
        ImGui::Text(str.c_str());
        ImGui::DragFloat("Size##CameraController", &m_frameSize, 0.1f);
    }
    ImGui::Separator();

    auto entityList = GetScene().m_registry.view<TagComponent>();

    static std::string filter;
    ImGui::InputText("Filter##CamerController", &filter);
    if(ImGui::ListBoxHeader("Entity"), entityList.size(), 4)
    {
        for(auto ent : entityList)
        {
            auto entity = GetEntityByHandle(ent);
            auto tagName = entity.getComponent<TagComponent>().tag;
            if( filter.empty() || String::isSubStringIgnoreCase(tagName, filter) )
            {
                if (ImGui::Selectable(tagName.c_str()))
                {
                    m_followEntityID = entity.getUUID();
                }
            }
        }
        ImGui::ListBoxFooter();
    }

}


}