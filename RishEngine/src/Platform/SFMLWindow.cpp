#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Clock.hpp>
//
#include <imgui.h>
#include <imgui-SFML.h>
//
#include "Rish/Core/Core.h"
#include "Rish/Platform/SFMLWindow.h"

#include "Rish/Events/ApplicationEvent.h"
#include "Rish/Events/KeyEvent.h"
#include "Rish/Events/MouseEvent.h"

namespace rl {

#define SFML_EVENT_CALLBACK(x) SFMLEventDispatcher::SFMLEventCallback x

Window* Window::Create(const std::string &title, uint32_t width, uint32_t height)
{
    return new SFMLWindow(title, width, height);
}

SFMLWindow::SFMLWindow(const std::string &title, const uint32_t width, const uint32_t height)
    : Window(title, width, height),
    m_SFMLWindow(sf::VideoMode(width, height), title)
{
    ImGui::SFML::Init(m_SFMLWindow);

    // Add SFML Event mapping
    SFML_EVENT_CALLBACK(closeEvent) = [&](const sf::Event &e) {
        RL_UNUSED(e);
        WindowCloseEvent windowClose;
        m_eventCallback(windowClose);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::Closed, closeEvent);

    // Resize event
    SFML_EVENT_CALLBACK(resizeEvent) = [&](const sf::Event &e) {
        WindowResizeEvent resize(e.size.width, e.size.height);
        m_eventCallback(resize);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::Resized, resizeEvent);

    // key pressed event
    SFML_EVENT_CALLBACK(keyPressedEvent) = [&](const sf::Event &e) {
        // keyPressedEvent p;
        RL_CORE_TRACE("Pressed = {} {} {} {} {}", e.key.code, e.key.control, e.key.shift, e.key.system, e.key.alt);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::KeyPressed, keyPressedEvent);

    // key released event
    SFML_EVENT_CALLBACK(keyReleasedEvent) = [&](const sf::Event &e) {
        RL_CORE_TRACE("Released = {} {} {} {} {}", e.key.code, e.key.control, e.key.shift, e.key.system, e.key.alt);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::KeyReleased, keyReleasedEvent);

    // mouse moved event
    SFML_EVENT_CALLBACK(mouseMovedEvent) = [&](const sf::Event &e) {
        MouseMovedEvent m(static_cast<float>(e.mouseMove.x), static_cast<float>(e.mouseMove.y));
        m_eventCallback(m);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseMoved, mouseMovedEvent);

    // mouse scrolled event
    SFML_EVENT_CALLBACK(mouseScrolledEvent) = [&](const sf::Event &e) {
        MouseScrolledEvent mc;
        // position
        mc.x = static_cast<float>(e.mouseWheelScroll.x);
        mc.y = static_cast<float>(e.mouseWheelScroll.y);
        // wheel direction
        if(e.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            mc.yOffset = e.mouseWheelScroll.delta;
        else if(e.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
            mc.xOffset = e.mouseWheelScroll.delta;
        m_eventCallback(mc);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseWheelScrolled, mouseScrolledEvent);

    // mouse button pressed
    SFML_EVENT_CALLBACK(mouseButtonPressedEvent) = [&](const sf::Event &e) {
        MouseButtonPressedEvent m(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
        m_eventCallback(m);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseButtonPressed, mouseButtonPressedEvent);

    // mouse button released
    SFML_EVENT_CALLBACK(mouseButtonReleaseEvent) = [&](const sf::Event &e) {
        MouseButtonReleasedEvent m(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
        m_eventCallback(m);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseButtonReleased, mouseButtonReleaseEvent);
}

SFMLWindow::~SFMLWindow()
{
    ImGui::SFML::Shutdown();
}

void SFMLWindow::onUpdate()
{
    RL_CORE_ASSERT(m_eventCallback, "Event Callback is not ready");

    m_SFMLWindow.clear(m_clearColor);
    //
    sf::Event event;
    while(m_SFMLWindow.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);
        m_SFMLEventDispatcher.handleEvent(event);
    }
    ImGui::SFML::Update(m_SFMLWindow, m_clock.restart());
}

void SFMLWindow::onDraw()
{
    m_SFMLWindow.pushGLStates();
    ImGui::SFML::Render(m_SFMLWindow);
    m_SFMLWindow.popGLStates();
    //
    m_SFMLWindow.display();
}

}
