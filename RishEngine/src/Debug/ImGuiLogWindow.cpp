#include <Rish/Debug/ImGuiLogWindow.h>

#include <IconsFontAwesome5.h>
#include <Rish/ImGui.h>
#include <imgui_stdlib.h>

ImGuiLogWindow defaultLogWindow;

void ImGuiLogWindow::onImGuiRender()
{
    // limit the log lines
    if(msgList.size() > logLinesLimit)
        msgList.resize(logLinesLimit);

    ImGui::Begin(ICON_FA_ALIGN_LEFT " Log");
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Filter"); ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputText("##Filter", &m_filterTag);
        //
        ImGui::SameLine();
        //
        ImGui::Text("Text"); ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputText("##FilterText", &m_filterText);
        //
        ImGui::SameLine();
        static bool autoScroll = true;
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        //
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        auto printMsg = [](std::string &msg)
        {
            ImGui::PushID(&msg);
            if( msg.find("info") != std::string::npos )
            {
                auto pos = msg.find("info");
                ImGui::Text("%s", msg.substr(0,pos).c_str());
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.f,1.f,0.f,1.f),"info");
                ImGui::SameLine();
                ImGui::Text("%s", msg.substr(pos+4,msg.size()-(pos+4)).c_str());
            }
            else if( msg.find("error") != std::string::npos )
            {
                auto pos = msg.find("error");
                ImGui::Text("%s", msg.substr(0,pos).c_str());
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f),"error");
                ImGui::SameLine();
                ImGui::Text("%s", msg.substr(pos+5,msg.size()-(pos+5)).c_str());
            }
            else if( msg.find("warning") != std::string::npos )
            {
                auto pos = msg.find("warning");
                ImGui::Text("%s", msg.substr(0,pos).c_str());
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.f,1.f,0.2f,1.f),"warning");
                ImGui::SameLine();
                ImGui::Text("%s", msg.substr(pos+7,msg.size()-(pos+7)).c_str());
            }
            else
            {
                ImGui::Text("%s", msg.c_str());
            }
            ImGui::PopID();
        };
        for (auto msg = msgList.rbegin(); msg != msgList.rend(); msg++)
        {
            // TODO: improve
            if(m_filterTag.empty() ||
               (msg->find("[" + m_filterTag + "]") != std::string::npos &&
               (m_filterText.empty() || msg->find(m_filterText) != std::string::npos))
            )
            {
                printMsg(*msg);
            }
        }
        ImGui::PopStyleVar();

        // Auto scrolling if the scroll bar is bottom
        if(autoScroll)
        {
            ImGui::SetScrollHereY(1.0f);
        }

        if(ImGui::GetScrollY() < ImGui::GetScrollMaxY())
        {
            autoScroll = false;
        }
        else if(ImGui::GetScrollY() == ImGui::GetScrollMaxY())
        {
            autoScroll = true;
        }

        ImGui::EndChild();
    }
    ImGui::End();
}
