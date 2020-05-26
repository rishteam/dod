/**
 * @file SFMLWindow.h
 * @author roy4801 (you@domain.com)
 * @brief Header of SFMLWindow class
 * @date 2020-05-25
 */
#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>

#include <Rish/Core/Window.h>

namespace rl {

/**
 * @brief SFMl Window class
 * @details Wraps the sf::RenderWindow and provides API
 */
class RL_API SFMLWindow final : public Window
{
public:
    SFMLWindow(const std::string &title, const uint32_t width, const uint32_t height);
    ~SFMLWindow() override;

    void onUpdate() override;
    void onDraw() override;

    void setEventCallback(const EventCallbackFunc &callback) override { m_eventCallback = callback; }

    bool isOpen() override { return m_SFMLWindow.isOpen(); }

    sf::RenderWindow& getWindowRef() { return m_SFMLWindow; }

private:
    sf::RenderWindow m_SFMLWindow;
    sf::Clock m_clock;
    sf::Color m_clearColor = sf::Color::Black;
    EventCallbackFunc m_eventCallback;
};

}