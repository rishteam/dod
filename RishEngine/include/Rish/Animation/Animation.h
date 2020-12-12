/**
 * @file Animation.h
 * @author roy4801 (me@roy4801.tw)
 * @brief Animation header
 * @date 2020-05-28
 */
#pragma once
#include "Rish/rlpch.h"

#include <SFML/Graphics.hpp>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/printf.h>
#include <Rish/ImGui/ImGui.h>

#include "Rish/Core/Core.h"

// TODO: set loop start end
// TODO: support flip

// Ref: https://www.sfml-dev.org/tutorials/2.5/graphics-transform.php

namespace rl {

class AnimationLoader;
class AnimationEditor;

// e.g.
// Animation test("../assets/reimu-hover.ani");

/**
 * @brief Animation class
 * @details
 */
class Animation
{
    friend class AnimationLoader;
    friend class AnimationEditor;
public:
    // Constructors
    Animation() = default;
    ~Animation() = default;

    void pause() { m_pause = true; }
    void resume() { m_pause = false; }
    void stop() { m_pause = true; m_nowFrame = 0; }
    bool isEnd();

    // the duration of the animation (second)
    float duration = 0.f;
    float reverseDuration = 0.f;
    bool loop = false;
    bool reverse = false;

    void addFrame(const sf::Texture &tex)
    {
        m_textureVec.push_back(tex);
    }
    bool removeFrame(int idx=-1)
    {
        // TODO: make tex std::list for optimizing
        int size = static_cast<int>(m_textureVec.size());
        if(idx == -1)
            idx = size-1;
        if(idx < size && idx != -1)
        {
            m_textureVec.erase(m_textureVec.begin() + idx);
            return true;
        }
        else
            return false;
    }

    const sf::Sprite& getSprite() const { return m_sprite; }
    const sf::Texture& getNowTexture() const { return m_nowFrame < m_textureVec.size() ? m_textureVec[m_nowFrame] : m_emptyTexture; }
    void setNowFrameCount(const int cnt) { m_nowFrame = cnt>=0 && cnt<m_count ? cnt : 0; }
    int getNowFrameCount() { return m_nowFrame; }
    int getTotalFrameCount() { return m_count; }
    /// The load type of the animation
    enum class LoadType : unsigned char
    {
        /// None load type
        AniLoadNone = 0,
        /// Load frames in spearate files
        AniLoadSeparate,
        /// Load frames in a sprite sheet
        AniLoadSpriteSheet,
        /// Total count
        AniLoadTypeTotal
    };
    LoadType m_loadType = LoadType::AniLoadNone;
private:
    // Texture attributes
    int m_count = 0;                //! Frame count
    mutable int m_nowFrame = 0;     //! Current frame
    std::string m_texName;          //! Texture prefix
    std::vector<sf::Texture> m_textureVec; //! Texture list
    sf::Texture m_emptyTexture; //! empty texture TODO: deprecate? need check
    mutable sf::Sprite m_sprite;//! Sprite for drawing
    //
    mutable sf::Clock m_clk; //! clock
    bool m_ready = false; //! is the animation ready

    bool m_pause = false; //! is the animation pause
};

}