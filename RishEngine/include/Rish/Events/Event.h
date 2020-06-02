/**
 * @file Event.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Event classes
 * @date 2020-05-23
 */
#pragma once

#include "Rish/rlpch.h"
#include "Rish/Core/Core.h"

#include <fmt/format.h>

namespace rl {

/**
 * @brief Event type enum class
 * @details Serve as the RTTI implement by ourselves
 */
enum class EventType
{
    None = 0,
    // Application Events
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    AppTick, AppUpdate, AppRender,
    // Keyboard Events
    KeyPressed, KeyReleased,
    // Mouse Events
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

/**
 * @brief Event Category bit flag enum
 * @details
 */
enum EventCategory
{
    None                     = 0,
    /// Application Event
    EventCategoryApplication = BIT(0),
    /// Input Event
    EventCategoryInput       = BIT(1),
    /// Keyboard Event
    EventCategoryKeyboard    = BIT(2),
    /// Mouse Event
    EventCategoryMouse       = BIT(3),
    /// Mouse Button Event
    EventCategoryMouseButton = BIT(4)
};

/**
 * @brief Event class type macro
 * @details Provide functionalities such as RTTI and debug name.
 * `getStaticType()` returns the type enum of that event, `getEventTyoe()` wraps the `getStaticType()`
 * And the `getEventType()` is polymorphic function so which version will the instance call is determined
 * in run-time. Hence, it implements simple RTTI.
 *
 * @warning Must add inside a declaration of a Event class **ONLY**
 */
#define EVENT_CLASS_TYPE(type)                                                  \
    static EventType getStaticType() { return EventType::type; }                \
    virtual EventType getEventType() const override { return getStaticType(); } \
    virtual const char* getName() const override { return #type; }

/**
 * @brief Event class category
 * @details Categorize the event class in run-time
 * @warning Must add inside a declaration of a Event class
 */
#define EVENT_CLASS_CATEGORY(category)            \
    virtual int getCategoryFlags() const override \
    {                                             \
        return category;                          \
    }

/**
 * @brief Event class toString macro
 * @warning Must add inside a declaration of a Event class
 */
#define EVENT_CLASS_TOSTRING(...)                 \
    virtual std::string toString() const override \
    {                                             \
        return fmt::format(__VA_ARGS__);          \
    }

/**
 * @brief Base Event Interface class
 * @details
 */
class RL_API Event
{
    friend class EventDispatcher;

public:
    virtual ~Event() {}
    // implement by macro in a sub-class
    virtual EventType getEventType() const = 0;
    virtual const char *getName() const = 0;
    virtual int getCategoryFlags() const = 0;
    //
    virtual std::string toString() const { return getName(); }

    inline bool isInCategory(EventCategory category)
    {
        return getCategoryFlags() & category;
    }

public:
    /// Is this event have been handled. (will not be propagated)
    bool handled = false;
};

/**
 * @brief Event Dispatcher
 * @details Dispatch the RishEngine Event to it handle function e.g. `on???Event(??? &e)`
 */
class EventDispatcher
{
    /// Event function typedef
    template <typename T>
    using EventFunc = std::function<bool(T &)>;

public:
    EventDispatcher(Event &event) : m_event(event)
    {
    }

    /**
     * @brief Dispatch the event by its type to the function provided
     *
     * @tparam T Event Type
     * @param eventFunc The function that handle this event
     * @return true succeeded
     * @return false failed
     */
    template <typename T>
    bool dispatch(EventFunc<T> eventFunc)
    {
        // if the type matches, calling the handle function
        if (m_event.getEventType() == T::getStaticType())
        {
            try
            {
                m_event.handled = eventFunc(dynamic_cast<T &>(m_event));
            }
            catch (const std::bad_cast &e)
            {
                RL_ASSERT(false, "{}", e.what()); // crash the application
            }
            return true;
        }
        return false;
    }
    // TODO: maybe constrain the type that can input in *Compile Time*

private:
    Event &m_event;
};

inline std::ostream &operator<<(std::ostream &os, const Event &e)
{
    return os << e.toString();
}

}