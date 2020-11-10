#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

#include <Rish/ImGui/ImGui.h>
#include <imgui-SFML.h>

#include "Rish/Core/Log.h"
#include "Rish/Core/ResManager.h"
#include "Rish/Core/ResHolder.h"

#include "Rish/Animation/Editor/EditorApp.h"
#include "Rish/Animation/Animation.h"
#include "Rish/Animation/Loader.h"

#include <filesystem>
namespace fs = std::filesystem;

using namespace rl;

int main()
{
    Logger::Init();

    fs::current_path(fs::current_path().parent_path());
    //
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "CSGO");
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    sf::Color bgColor(0, 0, 0);

    // auto &tex = ResHolder::Texture().load("test", "../assets/missing_texture.png");
    // sf::Sprite sp(tex);
    // auto &missTex = ResHolder::Texture()


    AnimationEditor animationEditor;
    // Animation test("assets/reimu-hover.ani");
    // test.setScale(sf::Vector2f(5.f, 5.f));

    //
    while (window.isOpen())
    {
        ///////////////////////////////////////
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            //
            animationEditor.processEvent(event);

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
        animationEditor.update();
        ImGui::Begin("Debug");
        {
            // test.debugImGuiWidgets();

            if(animationEditor.selectedOpenFile)
            {
                ImGui::Text(animationEditor.currentOpenFilePath.c_str());
            }
        }
        ImGui::End();
        // Game Update

        ///////////////////////////////////////
        // Draw
        window.clear(bgColor); // Clear screen

        // OpenGL draws
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glEnable(GL_BLEND);
        // glColor4f(1.0, 0.0, 0.0, 0.5);
        // glBegin(GL_TRIANGLES);
        //     glVertex2f(-0.5, -0.5);
        //     glVertex2f(0.0, 0.5);
        //     glVertex2f(0.5, -0.5);
        // glEnd();

        // SFML Draws
        window.pushGLStates();
            window.draw(animationEditor);
            // window.draw(test);
            // window.draw(sp);
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
