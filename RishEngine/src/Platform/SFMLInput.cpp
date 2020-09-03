#include <Rish/rlpch.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <Rish/Core/Application.h>
#include <Rish/Input/Input.h>

// TODO(roy4801): use RishEngine keycodes

namespace rl {

bool Input::s_keyboardState = true;
bool Input::s_mouseState = true;

bool Input::IsKeyPressed(int keycode)
{
    return s_keyboardState && sf::Keyboard::isKeyPressed((sf::Keyboard::Key)keycode);
}

bool Input::IsMouseButtonPressed(int mbutton)
{
    return s_mouseState && sf::Mouse::isButtonPressed((sf::Mouse::Button)mbutton);
}

std::pair<float, float> Input::GetMousePosition()
{
    auto *ptr = static_cast<sf::Window*>(Application::Get().getWindow().getPlatformWindow());
    sf::Vector2i pos = sf::Mouse::getPosition(*ptr);
    return std::make_pair((float)pos.x, (float)pos.y);
}

void Input::SetMousePosition(float x, float y)
{
    auto *ptr = static_cast<sf::Window*>(Application::Get().getWindow().getPlatformWindow());
    sf::Vector2i pos{static_cast<int>(x), static_cast<int>(y)};
    sf::Mouse::setPosition(pos, *ptr);
}

float Input::GetMouseX()
{
    return GetMousePosition().first;
}

float Input::GetMouseY()
{
    return GetMousePosition().second;
}

}