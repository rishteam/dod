#pragma once

#include <Rish/rlpch.h>

namespace ImGui {

class CloseableWindow
{
public:
    CloseableWindow();
    virtual ~CloseableWindow();

    virtual void onImGuiUpdate() = 0;
    void onImGuiRender();

    void showWindow() { m_showEditor = true; }
    void hideWindow() { m_showEditor = false; }
    void toggleWindow() { m_showEditor = !m_showEditor; }
private:
    bool m_showEditor = true;
};

} // end of namespace ImGui