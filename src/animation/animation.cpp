#include <fstream>

#include "animation/animation.h"
#include "animation/loader.h"

#include "resManager.h"
#include "log.h"

namespace rl {

// Default constructor of rl::animation
Animation::Animation()
{
}

Animation::Animation(const std::string &configPath)
    : Animation()
{
    loadConfig(configPath);
}

void Animation::loadConfig(const std::string &configPath)
{
    // Read file
    std::ifstream aniFile(configPath);
    std::string aniContent((std::istreambuf_iterator<char>(aniFile)), (std::istreambuf_iterator<char>()));
    //
    *this = AnimationLoader::loadFromString(aniContent);
}

void Animation::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    RL_ASSERT(m_ready, "Animation is not ready");
    float dur = reverse ? reverseDuration : duration;
    if (m_clk.getElapsedTime().asSeconds() >= dur / m_count)
    {
        m_clk.restart();
        if(!reverse) // normal play
            m_nowFrame++;
        else
            m_nowFrame--;
        if(!reverse) // normal play
        {
            if(m_nowFrame >= m_count)
            {
                if(loop)
                    m_nowFrame = 0;
                else
                    m_nowFrame = m_count-1;
            }
        }
        else
        {
            if(m_nowFrame < 0)
            {
                if(loop)
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
    RL_ASSERT(m_ready, "Animation is not ready");
    if(loop) return false;

    if(reverse)
        return m_nowFrame == 0;
    else
        return m_nowFrame == m_count-1;
}

}