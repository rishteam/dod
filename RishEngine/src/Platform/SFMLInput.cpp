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
    sf::Window &window = *static_cast<sf::Window*>(Application::Get().getWindow().getPlatformWindow());
    sf::Vector2f size = {(float)window.getSize().x / 2.f, (float)window.getSize().y / 2.f};
    sf::Vector2f pos = {(float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y};

    // To center
    pos.x = pos.x - size.x;
    pos.y = size.y - pos.y;

    // to NDC
    pos.x /= size.x;
    pos.y /= size.y;

    return std::make_pair(pos.x, pos.y);
}

void Input::SetMousePosition(float x, float y)
{
    sf::Window &window = *static_cast<sf::Window*>(Application::Get().getWindow().getPlatformWindow());
    sf::Vector2f pos{x, y};
    sf::Vector2f size = {(float)window.getSize().x / 2.f, (float)window.getSize().y / 2.f};

    pos.x *= size.x;
    pos.y *= size.y;

    pos.x = pos.x + size.x;
    pos.y = size.y - pos.y;

    sf::Mouse::setPosition({(int)pos.x, (int)pos.y}, window);
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