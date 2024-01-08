#include <Rish/ImGui/ImGui.h>

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
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
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

std::vector<char*> strList;
bool ImGui::Combo(const char *label, int *current_item, const std::vector<std::string> &v, int popup_max_height_in_items)
{
    // TODO: preformance?
    strList.resize(v.size());
    //
    for(int i = 0; i < v.size(); i++)
        strList[i] = (char*)v[i].c_str();
    bool res = Combo(label, current_item, &strList[0], v.size(), popup_max_height_in_items);
    return res;
}

bool ImGui::Combo(const char *label, int *current_item, const std::vector<std::string_view> &v,
                  int popup_max_height_in_items)
{
//    const char* *strList = new const char*[v.size()];
    strList.resize(v.size());
    //
    for(int i = 0; i < v.size(); i++)
        strList[i] = (char*)v[i].data();
    bool res = Combo(label, current_item, &strList[0], v.size(), popup_max_height_in_items);
    return res;
}

//
bool ImGui::ListBox(const char* label, int* current_item, const std::vector<std::string> &v, int height_in_items)
{
    /*if (ImGui::ListBoxHeader(label, v.size(), height_in_items))
    {
        for(int i = 0; i < v.size(); i++)
        {
            if(ImGui::Selectable(v[i].c_str(), *current_item == i))
            {
                *current_item = i;
            }
        }
        ImGui::ListBoxFooter();
    }*/

    // TODO(Roy): fix
    return false;
}

void ImGui::HelpMarker(const char *desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs)
{
    return ImVec2{lhs.x+rhs.x, lhs.y+rhs.y};
}

ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs)
{
    return ImVec2{lhs.x-rhs.x, lhs.y-rhs.y};
}

ImVec2 operator*(const ImVec2 &lhs, float f)
{
    return ImVec2{lhs.x * f, lhs.y * f};
}
