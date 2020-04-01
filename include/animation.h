#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include <fmt/core.h>
#include <fmt/printf.h>

#include <core.h>

// TODO: implement load resource info for origin and fps and scale and loop

// TODO: set loop start end
// TODO: support flip

// Ref: https://www.sfml-dev.org/tutorials/2.5/graphics-transform.php

namespace rl {

class Animation : public sf::Transformable
{
public:
    Animation();
    Animation(std::string texName, int count,
        std::string path, std::string fnameFmt,
        bool load=true);
    // TODO: impl
    void setInfo(std::string texName, int count,
        std::string path, std::string fnameFmt);

    void loadRes();
    void draw(sf::RenderTarget &target);

    // the duration of the animation (second)
    float duration, reverseDuration;
    bool loop;
    bool reverse;

    bool isEnd();

    struct AniInfo
    {
        float x, y;
        float width, height;
        float angle;
    };
    const AniInfo& getAnimationInfo()
    {
        RL_ASSERT(!m_spriteVec.empty(), "The sprite vector is empty\n");
        auto bound = m_spriteVec.back()->getLocalBounds();
        float size = std::max(fabs(getScale().x), fabs(getScale().y));
        static AniInfo info;
        info.x = bound.left;
        info.y = bound.top;
        info.width = bound.width * size;
        info.height = bound.height * size;
        info.angle = getRotation();
        return info;
    }

    const AniInfo& getOrigInfo()
    {
        RL_ASSERT(!m_spriteVec.empty(), "The sprite vector is empty\n");
        auto bound = m_spriteVec.back()->getLocalBounds();
        static AniInfo info;
        info.x = bound.left;
        info.y = bound.top;
        info.width = bound.width;
        info.height = bound.height;
        info.angle = getRotation();
        return info;
    }

    void resetFrame() { m_nowFrame = 0; }
    int getNowFrame() { return m_nowFrame; }

    int getCount() { return m_count; }
private:
    // Texture attributes
    int m_count, m_nowFrame;
    std::string m_texName;
    std::string m_format, m_path;
    std::vector<std::shared_ptr<sf::Sprite>> m_spriteVec; // sprite storage
    //
    sf::Clock m_clk;
    bool ready; // impl
};

}