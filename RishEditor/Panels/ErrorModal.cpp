#include "ErrorModal.h"

#include <Rish/ImGui.h>

namespace rl {

void rl::ErrorModal::onImGuiRender()
{
    if(m_showErrorModal)
    {
        if (!ImGui::IsPopupOpen("Something bad happened"))
            ImGui::OpenPopup("Something bad happened");
        // TODO: improve imgui modal data communication

        m_showErrorModal = false;
    }
    if (ImGui::BeginPopupModal("Something bad happened",
       nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("%s", m_message.c_str());
        if (ImGui::Button("Ok")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

} // end of namespace rl