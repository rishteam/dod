#include "animation/animation.h"

#include "resManager.h"
#include "log.h"

namespace rl {

// Default constructor of rl::animation
Animation::Animation()
{
    duration = reverseDuration = 1.f;
    m_count = m_nowFrame = 0;
    loop = true;
    reverse = false;

    ready = false;
}

// TODO: sprite rotation
// Refactor: Use only one sprite but set different texture.

// Parameter Constructor of rl::Animation
// Parameters:
// std::string texName  : texture name
// int count            : Number of textures
// std::string path     : Path to the files (Need a slash`/` in the end)
// std::string fnameFmt : filename format
// bool load=true       : If true, load resources in constructor. Otherwise it need to load manaually
Animation::Animation(std::string texName, int count,
    std::string path, std::string fnameFmt,
    bool load)
    : Animation()
{
    m_texName = texName;
    m_count   = count;
    m_path    = path;
    m_format  = fnameFmt;
    if(load) loadRes();
}

void Animation::setInfo(std::string texName, int count,
                        std::string path, std::string fnameFmt)
{
    m_texName = texName;
    m_count   = count;
    m_path    = path;
    m_format  = fnameFmt;
}

void Animation::loadRes()
{
    for(int i = 0; i < m_count; i++)
    {
        std::string tar = fmt::format("{}_{}", m_texName, i);
        std::string fullPath = m_path + fmt::format(m_format, i);
        if (!ResManager::loadRes(ResType::ResTexture, tar, fullPath))
        {
            RL_ERROR("Failed to load animation {} when loading texture {}\ntarget {}", m_texName, i, fullPath);
            m_spriteVec.clear();
            m_count = 0;
            return;
        }
        else
        {
            sf::Texture &tex = ResManager::getTexture(tar);
            m_spriteVec.push_back(std::make_shared<sf::Sprite>(tex));
            sf::Sprite &sprite = *m_spriteVec.back();
        }
    }
    ready = true;
}

void Animation::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    RL_ASSERT(ready, "Animation is not ready");
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
    sf::Sprite &now = *m_spriteVec[m_nowFrame];
    const sf::Transform &trans = getTransform();
    states.transform *= trans;
    target.draw(now, states);

    if(!debugDrawFlag) return;
    // draw origin point
    sf::Vector2f pos = getPosition();
    pos.x -= debugCirRadius; pos.y -= debugCirRadius;
    debugOrigCir.setRadius(debugCirRadius);
    debugOrigCir.setPosition(pos);
    debugOrigCir.setFillColor(sf::Color::Red);
    // draw sprite rect
    sf::FloatRect rect = now.getLocalBounds();
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
    if(loop) return false;

    if(reverse)
        return m_nowFrame == 0;
    else
        return m_nowFrame == m_count-1;
}

}