/**
 * @file Input.h
 * @author roy4801 (me@roy4801.tw)
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
    //////////////////////////////////////
    // Keyboard
    /// Get Key pressed
    static bool IsKeyPressed(int keycode);
    // Mouse
    /// Get Mouse button pressed
    static bool IsMouseButtonPressed(int mbutton);
    /// Get Mouse Position
    static std::pair<float, float> GetMousePosition();
    /// Set Moise Position
    static void SetMousePosition(float x, float y);
    //
    static float GetMouseX();
    static float GetMouseY();
    /// Enable/Disable Input
    static void SetInputState(bool state) { SetKeyboardState(state); SetMouseState(state); }
    /// Enable/Disable Keyboard
    static void SetKeyboardState(bool state) { s_keyboardState = state; }
    /// Enable/Disable Mouse
    static void SetMouseState(bool state) { s_mouseState = state; }
    /// Is in editor
    static void SetInEditor(bool state) { s_isInEditor = state; }

private:
    static bool s_keyboardState;
    static bool s_mouseState;
    //
    static bool s_isInEditor;
    static float s_x, s_y;
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