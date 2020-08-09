#include <Rish/rlpch.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <Rish/Core/Application.h>
#include <Rish/Platform/SFMLInput.h>

// TODO(roy4801): use RishEngine keycodes

namespace rl {

// Since the Input instance will be existed in the whole lifetime of the engine
// We don't bother to delete it
Scope<Input> Input::s_instance = MakeScope<SFMLInput>();

bool SFMLInput::isKeyPressedImpl(int keycode)
{
    return sf::Keyboard::isKeyPressed((sf::Keyboard::Key)keycode);
}

bool SFMLInput::isMouseButtonPressedImpl(int mbutton)
{
    return sf::Mouse::isButtonPressed((sf::Mouse::Button)mbutton);
}

std::pair<float, float> SFMLInput::getMousePositionImpl()
{
    sf::Window *ptr = static_cast<sf::Window*>(Application::Get().getWindow().getPlatformWindow());
    sf::Vector2i pos = sf::Mouse::getPosition(*ptr);
    return std::make_pair((float)pos.x, (float)pos.y);
}

float SFMLInput::getMouseXImpl()
{
    auto [x, y] = getMousePositionImpl();
    return x;
}

float SFMLInput::getMouseYImpl()
{
    auto [x, y] = getMousePositionImpl();
    return y;
}

}