/**
 * @file Input.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Real Time Input Interface
 * @date 2020-06-13
 */
#pragma once

#include <Rish/rlpch.h>

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
    static bool IsKeyPressed(int keycode) { return s_keyboardState && s_instance->isKeyPressedImpl(keycode); }
    // Mouse
    /// Get Mouse button pressed
    static bool IsMouseButtonPressed(int mbutton) { return s_mouseState && s_instance->isMouseButtonPressedImpl(mbutton); }
    /// Get Mouse Posistion
    static std::pair<float, float> GetMousePosition() { return s_instance->getMousePositionImpl(); }
    /// Get Mouse X
    static float GetMouseX() { return s_instance->getMouseXImpl(); }
    /// Get Mouse Y
    static float GetMouseY() { return s_instance->getMouseYImpl(); }
    /// Enable/Disable Input
    static void SetInputState(bool state) { SetKeyboardState(state); SetMouseState(state); }
    /// Enable/Disable Keyboard
    static void SetKeyboardState(bool state) { s_keyboardState = state; }
    /// Enable/Disable Mouse
    static void SetMouseState(bool state) { s_mouseState = state; }

protected:
    virtual bool isKeyPressedImpl(int keycode) = 0; ///< @note Implement these in Platform

    virtual bool isMouseButtonPressedImpl(int mbutton) = 0; ///< @note Implement these in Platform
    virtual std::pair<float, float> getMousePositionImpl() = 0; ///< @note Implement these in Platform
    virtual float getMouseXImpl() = 0; ///< @note Implement these in Platform
    virtual float getMouseYImpl() = 0; ///< @note Implement these in Platform

private:
    static Scope<Input> s_instance;
    static bool s_keyboardState;
    static bool s_mouseState;
};

}

/**
 * @class rl::Input
 *
 * Usage example for realtime Keyboard polling
 * @code
 * if(rl::Input::IsKeyPressed(rl::Keyboard::A))
 *     // Key A pressed
 * @endcode
 * Usage example for realtime Mouse polling
 * @code
 * if(rl::Input::IsMouseButtonPressed(sf::Mouse::Left))
 * {
 *     // pressed left mouse button
 * }
 * // Mouse position
 * auto [x, y] = rl::Input::GetMousePosition();
 * // Get single axis only
 * float x = rl::Input::GetMouseX();
 * @endcode
 *
 */