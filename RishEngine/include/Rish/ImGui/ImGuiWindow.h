#pragma once

#include <Rish/rlpch.h>

namespace rl {

// TODO: Provide the options for setting styles

class ImGuiWindow
{
public:
    ImGuiWindow();
    ImGuiWindow(const char *title);
    virtual ~ImGuiWindow();

    virtual void onImGuiUpdate() = 0;
    void onImGuiRender();

    void setTitle(const char *title) { m_title = title; }
    const char * getTitle() const { return m_title; }
    void showWindow() { m_showEditor = true; }
    void hideWindow() { m_showEditor = false; }
    void toggleWindow() { m_showEditor = !m_showEditor; }
private:
    bool m_showEditor = false;
    const char * m_title = "DefaultWindowName";
};

} // end of namespace ImGui