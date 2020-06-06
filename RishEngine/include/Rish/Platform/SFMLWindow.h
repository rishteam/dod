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
#include <imgui.h>
#include <imgui-SFML.h>

#include <Rish/Core/Window.h>
#include <Rish/Events/ApplicationEvent.h>
#include <Rish/Events/KeyEvent.h>
#include <Rish/Events/MouseEvent.h>

namespace rl {

/**
 * @brief SFML Event dispatcher
 * @details Dispatch the SFML Events to the RishEngine Events
 */
class RL_API SFMLEventDispatcher
{
public:
    SFMLEventDispatcher() = default;
    ~SFMLEventDispatcher() = default;

    using SFMLEventCallback = std::function<void(const sf::Event &)>;
    void addListener(sf::Event::EventType type, const SFMLEventCallback &callback)
    {
        m_eventHandlers[type] = callback;
    }

    void removeListener(sf::Event::EventType type) { m_eventHandlers.erase(type); }

    void handleEvent(const sf::Event &e)
    {
        // Deprecated events we just ignore them
        if(e.type == sf::Event::MouseWheelMoved)
            return;

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
 * @details Concrete class that implements rl::Window using SFML
 *         Wraps the sf::RenderWindow and provides API
 *         When a event happened in Window, Window class will create the RishEngine event
 *         and send it back to Application through event callback set by Application
 */
class RL_API SFMLWindow final : public Window
{
public:
    SFMLWindow(const std::string &title, const uint32_t width, const uint32_t height);
    virtual ~SFMLWindow() override;

    virtual void onUpdate() override;
    virtual void onDraw() override;

    /**
     * @brief Set the Event Callback object
     * @details This is used by Application class for setting the event callback.
     * @param callback Event Callback function
     */
    virtual void setEventCallback(const EventCallbackFunc &callback) override { m_eventCallback = callback; }

    virtual bool isOpen() override { return m_SFMLWindow.isOpen(); }
    virtual void* getPlatformWindow() const override { return (void*)(&m_SFMLWindow); }

    sf::RenderWindow& getWindow() { return m_SFMLWindow; }

    virtual void initImGui() override;
    virtual void shutdownImGui() override;
    virtual void updateImGui() override;
    virtual void renderImGui() override;

private:
    /// SFML render window
    sf::RenderWindow m_SFMLWindow;
    bool m_loadDefaultFont=true;
    /// clock
    sf::Clock m_clock;
    /// background color
    sf::Color m_clearColor = sf::Color::Black;
    /// event callback set by Application
    EventCallbackFunc m_eventCallback;
    /// dispatches the sf::Event to RishEngine Event
    SFMLEventDispatcher m_SFMLEventDispatcher;
};

}