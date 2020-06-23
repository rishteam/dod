/**
 * @brief Mousecode
 */
#pragma once

namespace rl {

/**
 * @brief Things that related to mouse
 */
namespace Mouse {

/**
 * @brief Mouse Buttons
 */
enum Button
{
    Left,     //!< The left mouse button
    Right,    //!< The right mouse button
    Middle,   //!< The middle (wheel) mouse button
    XButton1, //!< The first extra mouse button
    XButton2, //!< The second extra mouse button

    ButtonCount //!< Keep last -- the total number of mouse buttons
};

/**
 * @brief Mouse Wheels
 */
enum Wheel
{
    VerticalWheel,  //!< The vertical mouse wheel
    HorizontalWheel //!< The horizontal mouse wheel
};

} // end of namespace Mouse

} // end of namespace rl