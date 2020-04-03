#include <fstream>

#include "animation/animation.h"
#include "animation/loader.h"

#include "resManager.h"
#include "log.h"

namespace rl {

// Default constructor of rl::animation
Animation::Animation()
{
    duration = reverseDuration = 1.f;
    m_count = m_nowFrame = 0;
    m_loop = true;
    m_reverse = false;

    ready = false;
}

Animation::Animation(std::string configPath) :
    Animation()
{
    // Read file
    std::ifstream aniFile(configPath);
    std::string aniContent((std::istreambuf_iterator<char>(aniFile)), (std::istreambuf_iterator<char>()));
    //
    *this = AnimationLoader::loadFromString(aniContent);
}

// Operator
Animation& Animation::operator=(const Animation &other)
{
    m_count = other.m_count;
    m_nowFrame = other.m_nowFrame;
    m_texName = other.m_texName;
    m_format = other.m_format;
    m_path = other.m_path;
    m_textureVec = other.m_textureVec;
    m_emptyTexture = other.m_emptyTexture;
    m_sprite = other.m_sprite;
    m_clk = other.m_clk;
    ready = other.ready;
    return *this;
}

void Animation::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    RL_ASSERT(ready, "Animation is not ready");
    float dur = m_reverse ? reverseDuration : duration;
    if (m_clk.getElapsedTime().asSeconds() >= dur / m_count)
    {
        m_clk.restart();
        if(!m_reverse) // normal play
            m_nowFrame++;
        else
            m_nowFrame--;
        if(!m_reverse) // normal play
        {
            if(m_nowFrame >= m_count)
            {
                if(m_loop)
                    m_nowFrame = 0;
                else
                    m_nowFrame = m_count-1;
            }
        }
        else
        {
            if(m_nowFrame < 0)
            {
                if(m_loop)
                    m_nowFrame = m_count-1;
                else
                    m_nowFrame = 0;
            }
        }
    }
    m_sprite.setTexture(m_textureVec[m_nowFrame]);

    const sf::Transform &trans = getTransform();
    states.transform *= trans;
    target.draw(m_sprite, states);

    if(!debugDrawFlag) return;
    // draw origin point
    sf::Vector2f pos = getPosition();
    pos.x -= debugCirRadius; pos.y -= debugCirRadius;
    debugOrigCir.setRadius(debugCirRadius);
    debugOrigCir.setPosition(pos);
    debugOrigCir.setFillColor(sf::Color::Red);
    // draw sprite rect
    sf::FloatRect rect = m_sprite.getLocalBounds();
    debugRect.setPosition(sf::Vector2f(rect.left, rect.top));
    debugRect.setSize(sf::Vector2f(rect.width, rect.height));
    debugRect.setFillColor(sf::Color::Transparent);
    debugRect.setOutlineColor(sf::Color::Blue);
    debugRect.setOutlineThickness(debugRectThick);
    // actual draw
    target.draw(debugRect, states);
    target.draw(debugOrigCir);
}

// TODO: change the end point to loop points
bool Animation::isEnd()
{
    RL_ASSERT(ready, "Animation is not ready");
    if(m_loop) return false;

    if(m_reverse)
        return m_nowFrame == 0;
    else
        return m_nowFrame == m_count-1;
}

}