#include <Rish/rlpch.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <Rish/Core/Application.h>
#include <Rish/Input/Input.h>

// TODO(roy4801): use RishEngine keycodes

namespace rl {

bool Input::s_keyboardState = true;
bool Input::s_mouseState    = true;
//
bool Input::s_isInEditor    = false;

float Input::s_x = 0.f, Input::s_y = 0.f;
float Input::s_centerX, Input::s_centerY;
float Input::s_width, Input::s_height;

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
    if(s_isInEditor)
    {
        return std::make_pair(s_x, s_y);
    }
    else
    {
        sf::Window &window = *static_cast<sf::Window *>(Application::Get().getWindow().getPlatformWindow());
        sf::Vector2f size = {(float) window.getSize().x / 2.f, (float) window.getSize().y / 2.f};
        sf::Vector2f pos = {(float) sf::Mouse::getPosition(window).x, (float) sf::Mouse::getPosition(window).y};

        // To center
        pos.x = pos.x - size.x;
        pos.y = size.y - pos.y;

        // to NDC
        pos.x /= size.x;
        pos.y /= size.y;

        return std::make_pair(pos.x, pos.y);
    }
}

void Input::SetMousePosition(float x, float y)
{
    sf::Window &window = *static_cast<sf::Window *>(Application::Get().getWindow().getPlatformWindow());
    //
    if(s_isInEditor)
    {
        float setX = s_centerX + x * s_width / 2.f;
        float setY = s_centerY + -y * s_height / 2.f;

        sf::Mouse::setPosition({(int) setX, (int) setY}, window);
    }
    else
    {
        sf::Vector2f pos{x, y};
        sf::Vector2f size = {(float) window.getSize().x / 2.f, (float) window.getSize().y / 2.f};

        pos.x *= size.x;
        pos.y *= size.y;

        pos.x = pos.x + size.x;
        pos.y = size.y - pos.y;

        sf::Mouse::setPosition({(int) pos.x, (int) pos.y}, window);
    }
}

float Input::GetMouseX()
{
    return GetMousePosition().first;
}

float Input::GetMouseY()
{
    return GetMousePosition().second;
}

void Input::SetGameWindowInEditor(float centerX, float centerY, float width, float  height)
{
    s_centerX = centerX;
    s_centerY = centerY;
    s_width = width;
    s_height = height;
}

void Input::OnMouseMove(float x, float y)
{
    s_x = x;
    s_y = y;
}

}