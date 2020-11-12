#pragma once

#include <Rish/rlpch.h>

#include <Rish/Core/Time.h>

namespace rl {

class EditorLayer;

class RL_API Panel
{
public:
    Panel() = default;
    virtual ~Panel() = default;
    //
    virtual void onAttach()         {}
    virtual void onDetach()         {}
    virtual void onUpdate(Time dt)  {}
    virtual void onImGuiRender() = 0;
    //
    void showPanel() { m_show = true; }
    void hidePanel() { m_show = false; }
    void togglePanel() { m_show = !m_show; }
    //
    void setContext(EditorLayer *layer) { m_parent = layer; }
    bool& getShowHandle() { return m_show; }
protected:
    bool m_show = false;
    EditorLayer *m_parent = nullptr;
};

} // end of namespace rl


