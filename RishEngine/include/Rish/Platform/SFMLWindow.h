/**
 * @file SFMLWindow.h
 * @author roy4801 (you@domain.com)
 * @brief Header of SFMLWindow class
 * @date 2020-05-25
 */
#pragma once

#include <Rish/rlpch.h>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>

#include <Rish/Core/Window.h>

namespace rl {

// forward declatation
class RL_API SFMLEventDispatcher
{
public:
    SFMLEventDispatcher()
    {
    }
    ~SFMLEventDispatcher()
    {
    }

    using SFMLEventCallback = std::function<void(const sf::Event &)>;
    void addListener(sf::Event::EventType type, const SFMLEventCallback &callback)
    {
        m_eventHandlers[type] = callback;
    }

    void handleEvent(const sf::Event &e)
    {
        if(!m_eventHandlers.count(e.type))
        {
            RL_CORE_WARN("Unhandled event occured: {}", m_EventTypeName[e.type]);
            return;
        }
        m_eventHandlers[e.type](e);
    }
private:
    std::map<sf::Event::EventType, SFMLEventCallback> m_eventHandlers;

    // for debug use
    std::string m_EventTypeName [23] = {
        "Closed",
        "Resized",
        "LostFocus",
        "GainedFocus",
        "TextEntered",
        "KeyPressed",
        "KeyReleased",
        "MouseWheelMoved",
        "MouseWheelScrolled",
        "MouseButtonPressed",
        "MouseButtonReleased",
        "MouseMoved",
        "MouseEntered",
        "MouseLeft",
        "JoystickButtonPressed",
        "JoystickButtonReleased",
        "JoystickMoved",
        "JoystickConnected",
        "JoystickDisconnected",
        "TouchBegan",
        "TouchMoved",
        "TouchEnded",
        "SensorChanged"
    };
};

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
    SFMLEventDispatcher m_SFMLEventDispatcher;
};

}