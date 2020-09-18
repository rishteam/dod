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

ImVec2 ImGui::GetMousePosRelatedToWindowNormalize()
{
    ImVec2 pos = GetMousePosRelatedToWindow();
    ImVec2 size = GetContentRegionAvail();
    // to NDC
    pos.x /= size.x;
    pos.y /= size.y;
    return pos;
}

ImVec2 ImGui::GetMousePosRelatedToWindowNormalizeCenter()
{
    ImVec2 pos = GetMousePosRelatedToWindow();
    ImVec2 size = GetContentRegionAvail();
    size.x /= 2.f;
    size.y /= 2.f;
    // To center
    pos.x = pos.x - size.x;
    pos.y = size.y - pos.y;
    // to NDC
    pos.x /= size.x;
    pos.y /= size.y;
    return pos;
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

void ImGui::BeginDockspace(const char *name)
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(name, nullptr, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
}

void ImGui::EndDockspace()
{
    ImGui::End();
}

ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs)
{
    return ImVec2{lhs.x+rhs.x, lhs.y+rhs.y};
}

ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs)
{
    return ImVec2{lhs.x-rhs.x, lhs.y-rhs.y};
}
