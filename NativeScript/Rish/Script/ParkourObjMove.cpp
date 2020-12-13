#include "ParkourObjMove.h"

namespace rl{

void ParkourObjMove::onCreate()
{

}

void ParkourObjMove::onDestroy()
{

}

void ParkourObjMove::onUpdate(Time dt)
{

    if(m_move.size() == 0)
        return;

    if(m_move.size() <= m_nowMove)
    {
        m_nowMove = 0;
        m_counter = 0.f;
    }

    glm::vec2 moveMent = m_move[m_nowMove].first;
    float limit =  m_move[m_nowMove].second;
    auto &pos = GetComponent<TransformComponent>().translate;

    if( m_counter < limit )
    {
        m_counter += dt.asSeconds();
        pos.x += moveMent.x*dt.asSeconds();
        pos.y += moveMent.y*dt.asSeconds();
    }
    else
    {
        m_nowMove++;
        m_counter = 0.f;
    }

}

void ParkourObjMove::onImGuiRender()
{

    ImGui::PushItemWidth(250);
    for(int i = 0 ; i < m_move.size(); i++ )
    {
        ImGui::PushID(i);
        ImGui::DragFloat2("Speed##Objmove",glm::value_ptr(m_move[i].first));
        ImGui::DragFloat("Time##Objmove",&m_move[i].second);
        ImGui::PopID();
    }
    ImGui::PopItemWidth();
    ImGui::Separator();

    if(ImGui::Button(ICON_FA_PLUS, ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
    {
        m_move.push_back(std::make_pair(glm::vec2(0.f,0.f),0.f));
    }

}

}
