#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include <fmt/core.h>
#include <fmt/printf.h>

#include <imgui.h>

#include <core.h>

// TODO: set loop start end
// TODO: support flip

// Ref: https://www.sfml-dev.org/tutorials/2.5/graphics-transform.php

namespace rl {

class AnimationLoader;

// e.g.
// Animation test("reimu-hover", 4, "assets/", "reimu-hover{}.png");

class Animation : public sf::Transformable, public sf::Drawable
{
    friend class AnimationLoader;
public:
    // Constructors
    Animation();

    // Load animation from a config file
    Animation(std::string configPath);

    // Operators
    Animation& operator=(const Animation &other);

    // Main function
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    // the duration of the animation (second)
    float duration, reverseDuration;
    bool m_loop;
    bool m_reverse;

    bool isEnd();

    void addFrame(const sf::Texture &tex)
    {
        m_textureVec.push_back(tex);
    }
    bool removeFrame(int idx=-1)
    {
        if(idx == -1)
            idx = m_textureVec.size()-1;
        if(idx < m_textureVec.size() && idx != -1)
        {
            m_textureVec.erase(m_textureVec.begin() + idx);
            return true;
        }
        else
            return false;
    }

    const sf::Sprite& getSprite() const { return m_sprite; }
    const sf::Texture& getNowTexture() const { return m_nowFrame < m_textureVec.size() ? m_textureVec[m_nowFrame] : m_emptyTexture; }
    void resetNowFrameCount() { m_nowFrame = 0; }
    int getNowFrameCount() { return m_nowFrame; }
    int getTotalFrameCount() { return m_count; }
private:
    // Texture attributes
    int m_count;                // Frame count
    mutable int m_nowFrame;     // Current frame
    std::string m_texName;      // Texture prefix
    std::string m_format;       // Filename format
    std::string m_path;         // Path to the files
    // TODO: change this to std::list
    std::vector<sf::Texture> m_textureVec; // Texture list
    sf::Texture m_emptyTexture; // empty texture
    mutable sf::Sprite m_sprite;// Sprite for drawing
    //
    mutable sf::Clock m_clk; // clock
    bool ready; // is the animation ready

// For Debug ONLY
public:
    bool debugDrawFlag = false;

    void debugImGuiWindow()
    {
        ImGui::Begin(m_texName.c_str());
        debugImGuiWidgets();
        ImGui::End();
    }
    void debugImGuiWidgets()
    {
        ImGui::Checkbox("draw", &debugDrawFlag);
        // Position
        float pos[2] = {getPosition().x, getPosition().y};
        ImGui::DragFloat2("Position", pos, 1.f);
        setPosition(pos[0], pos[1]);
        // Rotate
        float rotate = getRotation();
        ImGui::SliderFloat("Rotate", &rotate, 0.f, 360.f);
        setRotation(rotate);
        // Origin
        float ori[2] = {getOrigin().x, getOrigin().y};
        ImGui::DragFloat2("Origin", ori, 1.f);
        setOrigin(ori[0], ori[1]);
    }

    mutable sf::CircleShape debugOrigCir;
    float debugCirRadius = 5.f;
    mutable sf::RectangleShape debugRect;
    float debugRectThick = 1.f;

// print
public:
    friend std::ostream &operator<<(std::ostream& os, const Animation &ani)
    {
        std::string info = fmt::format("<Animation \"{}\" {}>", ani.m_texName, ani.m_count);
        os << info;
        return os;
    }
};

}