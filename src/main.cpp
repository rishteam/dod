#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

#include <imgui.h>
#include <imgui-SFML.h>
#include <nlohmann/json.hpp>

#include "log.h"
#include "resManager.h"
#include "animation.h"

using json = nlohmann::json;
using namespace rl;

int main()
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CSGO");
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    sf::Color bgColor(0, 0, 0);

    ResManager::setRootPath("/Users/roy4801/Desktop/Program/rish/dod/");

    Animation test("reimu-hover", 4, "assets/", "reimu-hover{}.png");

    test.setScale(sf::Vector2f(5.f, 5.f));

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
            ImGui::Checkbox("draw", &test.debugDrawFlag);
            // Position
            float pos[2] = {test.getPosition().x, test.getPosition().y};
            ImGui::DragFloat2("Position", pos, 1.f);
            test.setPosition(pos[0], pos[1]);
            // Rotate
            float rotate = test.getRotation();
            ImGui::SliderFloat("Rotate", &rotate, 0.f, 360.f);
            test.setRotation(rotate);
            // Origin
            float ori[2] = {test.getOrigin().x, test.getOrigin().y};
            ImGui::DragFloat2("Origin", ori, 1.f);
            test.setOrigin(ori[0], ori[1]);
        ImGui::End();
        // Game Update

        ///////////////////////////////////////
        // Draw
        window.clear(bgColor); // Clear screen

        // OpenGL draws
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glColor4f(1.0, 0.0, 0.0, 0.5);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.5, -0.5);
            glVertex2f(0.0, 0.5);
            glVertex2f(0.5, -0.5);
        glEnd();

        // SFML Draws
        window.pushGLStates();
            window.draw(test);
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