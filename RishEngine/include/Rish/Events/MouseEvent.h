#pragma once

#include "Rish/Events/Event.h"

namespace rl {

class RL_API MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(float x, float y)
        : m_x(x), m_y(y)
    {
    }

    inline float getX() const { return m_x; }
    inline float getY() const { return m_y; }
    inline std::pair<float, float> getPair() const {
        return std::make_pair(m_x, m_y);
    }

    EVENT_CLASS_TOSTRING("MouseMovedEvent: x={:4.2f} y={:4.2f}", m_x, m_y)

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
    float m_x, m_y;
};

class RL_API MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : m_xOffset(xOffset), m_yOffset(yOffset)
    {
    }

    inline float getXOffset() const { return m_xOffset; }
    inline float getYOffset() const { return m_yOffset; }
    inline std::pair<float, float> getPair() const
    {
        return std::make_pair(m_xOffset, m_yOffset);
    }

    EVENT_CLASS_TOSTRING("MouseScrolledEvent: xOff={:4.2f} yOff={:4.2f}", m_xOffset, m_yOffset)

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float m_xOffset, m_yOffset;
};

/**
 * @brief Mouse Button Event Interface
 * @warning This class is for interface ONLY, do not attempt to construct it
 */
class RL_API MouseButtonEvent : public Event
{
public:
    inline int getMouseButton() const { return m_button; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

protected:
    MouseButtonEvent(int btn)
        : m_button(btn)
    {
    }

    int m_button;
};

class RL_API MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(int button) : MouseButtonEvent(button)
    {
    }

    EVENT_CLASS_TOSTRING("MouseButtonPressedEvent: btn={}", m_button)

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class RL_API MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(int button) : MouseButtonEvent(button)
    {
    }

    EVENT_CLASS_TOSTRING("MouseButtonReleasedEvent: btn={}", m_button)

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

}