/**
 * @file ImGuiLayer.h
 * @author roy4801 (me@roy4801.tw)
 * @brief Header of ImGuiLayer
 * @date 2020-05-31
 */
#pragma once

#include <Rish/Core/Core.h>
#include <Rish/Core/Window.h>
#include <Rish/Core/Time.h>
#include <Rish/Layer/Layer.h>

#include <Rish/Events/Event.h>
#include <Rish/Events/ApplicationEvent.h>
#include <Rish/Events/KeyEvent.h>
#include <Rish/Events/MouseEvent.h>

namespace rl {

/**
 * @brief ImGui Layer
 * @ingroup core
 * @details Handles the ImGui layer things
 */
class RL_API ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() override;

    void onAttach() override;
    void onDetach() override;
    /// Begin imgui frame
    void begin();
    /// End imgui frame
    void end();

    void onEvent(Event &event) override;

    void onUpdate(Time dt) override {}
    void onImGuiRender() override {}

private:
    bool onMouseButtonPressed(MouseButtonPressedEvent &event);
    bool onMouseButtonReleased(MouseButtonReleasedEvent &event);
    bool onMouseMoved(MouseMovedEvent &event);
    bool onMouseScrolled(MouseScrolledEvent &event);
    bool onKeyPressed(KeyPressedEvent &event);
    bool onKeyReleased(KeyReleasedEvent &event);
    bool onKeyTyped(KeyTypedEvent &event);
    bool onWindowResize(WindowResizeEvent &event);
private:
    Window &m_window;
};

}
