#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/OpenGL.hpp>
//
#include <imgui.h>
#include <imgui-SFML.h>
//
#include <Rish/Platform/SFMLWindow.h>

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
}

SFMLWindow::~SFMLWindow()
{
    ImGui::SFML::Shutdown();
}

void SFMLWindow::onUpdate()
{
    m_SFMLWindow.clear(m_clearColor);
    //
    sf::Event event;
    while(m_SFMLWindow.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);

        // TODO: temp
        if(event.type == sf::Event::Closed)
            m_SFMLWindow.close();
        else if(event.type == sf::Event::Resized)
            glViewport(0, 0, event.size.width, event.size.height);
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

void SFMLWindow::setEventCallback(const EventCallbackFunc &callback)
{
}

}
