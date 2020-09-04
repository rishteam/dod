#include <Rish/Debug/ImGuiLogWindow.h>

#include <IconsFontAwesome5.h>
#include <imgui.h>

ImGuiLogWindow defaultLogWindow;

void ImGuiLogWindow::onImGuiRender()
{
    // limit the log lines
    if(msgList.size() > logLinesLimit)
        msgList.resize(logLinesLimit);

    ImGui::Begin(ICON_FA_ALIGN_LEFT " Log");
    {
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (auto i = msgList.rbegin(); i != msgList.rend(); i++)
        {
            ImGui::PushID(&*i);
            ImGui::Text("%s", i->c_str());
            ImGui::PopID();
        }
        ImGui::PopStyleVar();
        // Auto scrolling if the scroll bar is bottom
        if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
    ImGui::End();
}
