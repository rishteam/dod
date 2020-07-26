/**
 * @file KeyEvent.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Keyboard events
 * @date 2020-05-23
 */
#pragma once

#include <Rish/Core/Core.h>
#include "Rish/Events/Event.h"

namespace rl {

/**
 * @brief Key Event Interface
 * @warning This class is for interface **ONLY**, do not attempt to construct it
 */
class RL_API KeyEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);
protected:
    KeyEvent(int keycode) :
        keyCode(keycode)
    {
    }

public:
    int keyCode;
};

/**
 * @brief Key pressed event
 * @details
 */
class RL_API KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(int keycode_, int repeat_) :
        KeyEvent(keycode_),
        repeat(repeat_)
    {
    }

    EVENT_CLASS_TOSTRING("KeyPressEvent: keycode={} repeat={}", keyCode, repeat)
    EVENT_CLASS_TYPE(KeyPressed)

public:
    int repeat;
};

/**
 * @brief Key released event
 * @details
 */
class RL_API KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(int keycode_) :
        KeyEvent(keycode_)
    {
    }

    EVENT_CLASS_TOSTRING("KeyReleaseEvent: keycode={}", keyCode)

    EVENT_CLASS_TYPE(KeyReleased)
};

/**
 * @brief Key Typed Event
 * @details
 */
class RL_API KeyTypedEvent : public KeyEvent
{
public:
    KeyTypedEvent(int keycode_) : KeyEvent(keycode_)
    {}

    EVENT_CLASS_TOSTRING("KeyTypedEvent: keycode={}", keyCode)

    EVENT_CLASS_TYPE(KeyTyped)
};

}