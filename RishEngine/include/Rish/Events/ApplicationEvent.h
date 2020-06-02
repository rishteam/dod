/**
 * @file ApplicationEvent.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Application Events
 * @date 2020-05-28
 */
#pragma once

#include "Rish/Events/Event.h"

namespace rl {

/**
 * @brief Window Resize Event
 * @details With width and height
 */
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

public:
    uint32_t m_width, m_height;
};

/**
 * @brief Window Close Event
 * @details
 */
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

/**
 * @brief Window Focus Event
 * @details
 */
class RL_API WindowFocusEvent : public Event
{
    WindowFocusEvent()
    {
    }

    EVENT_CLASS_TOSTRING("WindowFocusEvent")

    EVENT_CLASS_TYPE(WindowFocus)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

/**
 * @brief Window Lost Focus Event
 * @details
 */
class RL_API WindowLostFocusEvent : public Event
{
    WindowLostFocusEvent()
    {
    }

    EVENT_CLASS_TOSTRING("WindowLostFocusEvent")

    EVENT_CLASS_TYPE(WindowLostFocus)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

/**
 * @brief Window Moved Event
 * @details
 */
class RL_API WindowMoved : public Event
{
    WindowMoved()
    {
    }

    EVENT_CLASS_TOSTRING("WindowMoved")

    EVENT_CLASS_TYPE(WindowMoved)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

/**
 * @brief App Tick
 */
class RL_API AppTickEvent : public Event
{
public:
    AppTickEvent() = default;

    EVENT_CLASS_TOSTRING("AppTickEvent:")

    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

/**
 * @brief App Update
 */
class RL_API AppUpdateEvent : public Event
{
public:
    AppUpdateEvent() = default;

    EVENT_CLASS_TOSTRING("AppUpdateEvent:")

    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

/**
 * @brief App Render
 */
class RL_API AppRenderEvent : public Event
{
public:
    AppRenderEvent() = default;

    EVENT_CLASS_TOSTRING("AppRenderEvent:")

    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

}