/**
 * @file KeyEvent.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Keyboard events
 * @date 2020-05-23
 */
#pragma once

#include "Rish/Events/Event.h"

namespace rl {

/**
 * @brief Key Event Interface
 * @warning This class is for interface ONLY, do not attempt to construct it
 */
class RL_API KeyEvent : public Event
{
public:
    inline int getKeyCode() const
    {
        return m_keyCode;
    }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);
protected:
    KeyEvent(int keycode) : m_keyCode(keycode)
    {
    }

    int m_keyCode;
};

/**
 * @brief Key pressed event
 * @details
 */
class RL_API KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(int keycode, int repeat) : KeyEvent(keycode)
        , m_repeat(repeat)
    {
    }

    inline int getRepeatCount() const { return m_repeat; }

    EVENT_CLASS_TOSTRING("KeyPressEvent: keycode={} repeat={}", m_keyCode, m_repeat)

    EVENT_CLASS_TYPE(KeyPressed)
private:
    int m_repeat;
};

/**
 * @brief Key released event
 * @details
 */
class RL_API KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(int keycode) : KeyEvent(keycode)
    {
    }

    EVENT_CLASS_TOSTRING("KeyReleaseEvent: keycode={}", m_keyCode)

    EVENT_CLASS_TYPE(KeyReleased)
};

}