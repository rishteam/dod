#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "log.h"
#include "resManager.h"

using namespace rl;

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CSGO");
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    sf::Color bgColor(0, 0, 0);

    Logger::setLogLevel(LOG_ERR);

    ResManager::setRootPath("C:/Users/Rish/Desktop/rish/dod");
    ResManager::loadRes(ResType::ResMusic, "main-bg", "assets/a.ogg");

    auto &main_bg = ResManager::getMusic("main-bg");
    main_bg.play();

    while (window.isOpen())
    {
        ///////////////////////////////////////
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ///////////////////////////////////////
        // Update
        // ImGui
        ImGui::Begin("Debug");
            ImGui::Text("test");
        ImGui::End();
        // Game Update

        ///////////////////////////////////////
        // Draw
        window.clear(bgColor); // Clear screen

        // OpenGL draws
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.5, -0.5);
            glVertex2f(0.0, 0.5);
            glVertex2f(0.5, -0.5);
        glEnd();

        // SFML Draws
        window.pushGLStates();

        window.popGLStates();

        // imgui draws
        window.pushGLStates();
            ImGui::SFML::Render(window);
        window.popGLStates();

        // Update the window
        window.display();
    }
    ImGui::SFML::Shutdown();
}