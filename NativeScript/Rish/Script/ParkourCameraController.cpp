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

    if( m_followEntityID.to_string() == "None" )
        return;

    auto &scene = GetScene();

    auto ent = scene.getEntityByUUID(m_followEntityID);

    auto &cameraTrans = GetComponent<TransformComponent>();
    auto &followTrans = ent.getComponent<TransformComponent>();

    cameraTrans.translate = followTrans.translate;

}

void ParkourCameraController::onImGuiRender()
{

    if( m_followEntityID.to_string() == "None" )
        ImGui::Text("Don't have followed Entity");

    auto entityList = GetScene().m_registry.view<TagComponent>();

    std::string str = "Now Follow is " + m_followEntityID.to_string();
    ImGui::Text(str.c_str());

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