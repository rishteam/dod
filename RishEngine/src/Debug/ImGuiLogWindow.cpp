#include <Rish/Debug/ImGuiLogWindow.h>

#include <IconsFontAwesome5.h>
#include <imgui.h>
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
            ImGui::Text("%s", msg.c_str());
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

        if(ImGui::GetScrollY() < ImGui::GetScrollMaxY())
        {
            autoScroll = false;
        }
        else if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            autoScroll = true;
        }
        // Auto scrolling if the scroll bar is bottom
        if(autoScroll)
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
    }
    ImGui::End();
}
