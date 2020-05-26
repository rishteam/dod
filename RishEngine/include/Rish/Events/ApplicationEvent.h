#pragma once

#include "Rish/Events/Event.h"

namespace rl {

class RL_API WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(uint32_t width, uint32_t height)
        : m_width(width), m_height(height)
    {
    }

    EVENT_CLASS_TOSTRING("WindowResizeEvent: w={:4d} h={:4d}", m_width, m_height)

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

// private:
    uint32_t m_width, m_height;
};

class RL_API WindowCloseEvent : public Event
{
public:
    WindowCloseEvent()
    {
    }

    EVENT_CLASS_TOSTRING("WindowCloseEvent: close")

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class RL_API AppTickEvent : public Event
{
public:
    AppTickEvent() = default;

    EVENT_CLASS_TOSTRING("AppTickEvent:")

    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class RL_API AppUpdateEvent : public Event
{
public:
    AppUpdateEvent() = default;

    EVENT_CLASS_TOSTRING("AppUpdateEvent:")

    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class RL_API AppRenderEvent : public Event
{
public:
    AppRenderEvent() = default;

    EVENT_CLASS_TOSTRING("AppRenderEvent:")

    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

}