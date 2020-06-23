/**
 * @file Input.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Real Time Input Interface
 * @date 2020-06-13
 */
#pragma once

#include <Rish/rlpch.h>

#include <Rish/Core/Core.h>
#include <Rish/Input/KeyCode.h>
#include <Rish/Input/MouseCode.h>

namespace rl {

// TODO: set mouse position?

/**
 * @brief Input interface
 * @warning Abstract class<br>Need to be inherited
 */
class RL_API Input
{
public:
    virtual ~Input() = default;
    // Key
    /// Get Key pressed
    static bool isKeyPressed(int keycode) { return s_instance->isKeyPressedImpl(keycode); }
    // Mouse
    /// Get Mouse button pressed
    static bool isMouseButtonPressed(int mbutton) { return s_instance->isMouseButtonPressedImpl(mbutton); }
    /// Get Mouse Posistion
    static std::pair<float, float> getMousePosition() { return s_instance->getMousePositionImpl(); }
    /// Get Mouse X
    static float getMouseX() { return s_instance->getMouseXImpl(); }
    /// Get Mouse Y
    static float getMouseY() { return s_instance->getMouseYImpl(); }

protected:
    virtual bool isKeyPressedImpl(int keycode) = 0; //! @note Implement these in Platform

    virtual bool isMouseButtonPressedImpl(int mbutton) = 0; //! @note Implement these in Platform
    virtual std::pair<float, float> getMousePositionImpl() = 0; //! @note Implement these in Platform
    virtual float getMouseXImpl() = 0; //! @note Implement these in Platform
    virtual float getMouseYImpl() = 0; //! @note Implement these in Platform

private:
    static Input *s_instance; //! Instance
};

}

/**
 * @class rl::Input
 *
 * Usage example for realtime Keyboard polling
 * @code
 * if(rl::Input::isKeyPressed(rl::Keyboard::A))
 *     // Key A pressed
 * @endcode
 * Usage example for realtime Mouse polling
 * @code
 * if(rl::Input::isMouseButtonPressed(sf::Mouse::Left))
 * {
 *     // pressed left mouse button
 * }
 * // Mouse position
 * auto [x, y] = rl::Input::getMousePosition();
 * // Get single axis only
 * float x = rl::Input::getMouseX();
 * @endcode
 *
 */