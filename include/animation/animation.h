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
// Animation test("../assets/reimu-hover.ani");

class Animation : public sf::Transformable, public sf::Drawable
{
    friend class AnimationLoader;
public:
    // Constructors
    Animation();

    // Load animation from a config file
    Animation(const std::string &configPath);

    // Load animation config
    void loadConfig(const std::string &configPath);

    // Main function
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    // the duration of the animation (second)
    float duration = 0.f;
    float reverseDuration = 0.f;
    bool loop = false;
    bool reverse = false;

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
    void setNowFrameCount(const int cnt) { if(cnt > 0 && cnt < m_count) m_nowFrame = 0; }
    int getNowFrameCount() { return m_nowFrame; }
    int getTotalFrameCount() { return m_count; }
    // The load type of the animation
    enum class LoadType : unsigned char
    {
        AniLoadNone = 0,
        AniLoadSeparate,    // Load frames in spearate files
        AniLoadSpriteSheet, // Load frames in a sprite sheet
        AniLoadTypeTotal
    };
    LoadType m_loadType = LoadType::AniLoadNone;
private:
    // Texture attributes
    int m_count = 0;                // Frame count
    mutable int m_nowFrame = 0;     // Current frame
    std::string m_texName;          // Texture prefix
    // TODO: change this to std::list
    std::vector<sf::Texture> m_textureVec; // Texture list
    sf::Texture m_emptyTexture; // empty texture
    mutable sf::Sprite m_sprite;// Sprite for drawing
    //
    mutable sf::Clock m_clk; // clock
    bool m_ready = false; // is the animation ready

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
        std::string info = fmt::format("<Animation \"{}\" {}>\n", ani.m_texName, ani.m_count);
        info += fmt::format("  ready={}\n", ani.m_ready);
        os << info;
        return os;
    }
};

}