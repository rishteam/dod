#include <Rish/ImGui.h>

ImVec2 ImGui::GetWindowContentSize()
{
    auto [ul, br] = GetWindowContentPoints();
    return ImVec2{br.x-ul.x, br.y-ul.y};
}

ImVec2 ImGui::GetMousePosRelatedToWindow()
{
    auto [ul, br] = GetWindowContentPoints();
    ImVec2 mpos = ImGui::GetMousePos();

    return ImVec2{mpos.x - ul.x, mpos.y - ul.y};
}

std::pair<ImVec2, ImVec2> ImGui::GetWindowContentPoints()
{
    ImVec2 padding = ImGui::GetStyle().WindowPadding;
    ImVec2 wpos = ImGui::GetWindowPos();
    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();

//    ImGui::GetForegroundDrawList()->AddRect(vMin+wpos, vMax+wpos, IM_COL32(255, 255, 0, 255));

    vMin.x -= padding.x;
    vMin.y -= padding.y;
    vMax.x += padding.x;
    vMax.y += padding.y;

//    ImGui::GetForegroundDrawList()->AddRect(vMin+wpos, vMax+wpos, IM_COL32(255, 0, 0, 255));

    vMin.x += wpos.x;
    vMin.y += wpos.y;
    vMax.x += wpos.x;
    vMax.y += wpos.y;

    return std::make_pair(vMin, vMax);
}

ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs)
{
    return ImVec2{lhs.x+rhs.x, lhs.y+rhs.y};
}

ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs)
{
    return ImVec2{lhs.x-rhs.x, lhs.y-rhs.y};
}
