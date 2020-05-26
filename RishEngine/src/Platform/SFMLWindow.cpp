#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Clock.hpp>
//
#include <imgui.h>
#include <imgui-SFML.h>
//
#include "Rish/Core/Core.h"
#include "Rish/Platform/SFMLWindow.h"

#include "Rish/Events/ApplicationEvent.h"

namespace rl {

Window* Window::Create(const std::string &title, uint32_t width, uint32_t height)
{
    return new SFMLWindow(title, width, height);
}

SFMLWindow::SFMLWindow(const std::string &title, const uint32_t width, const uint32_t height)
    : Window(title, width, height),
    m_SFMLWindow(sf::VideoMode(width, height), title)
{
    ImGui::SFML::Init(m_SFMLWindow);

    SFMLEventDispatcher::SFMLEventCallback closeEvent = [&](const sf::Event &e) {
        WindowCloseEvent windowClose;
        m_eventCallback(windowClose);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::Closed, closeEvent);

    SFMLEventDispatcher::SFMLEventCallback resizeEvent = [&](const sf::Event &e) {
        WindowResizeEvent resize(e.size.width, e.size.height);
        m_eventCallback(resize);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::Resized, resizeEvent);
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
