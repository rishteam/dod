#include <Rish/rlpch.h>
#include <Rish/Sound/Listener.h>

#include <SFML/Audio.hpp>

namespace rl {

void Listener::SetPosition(const glm::vec3 &pos)
{
    sf::Listener::setPosition(pos.x, pos.y, pos.z);
}

glm::vec3 Listener::GetPosition()
{
    auto pos = sf::Listener::getPosition();
    return glm::vec3{pos.x, pos.y, pos.z};
}

void Listener::SetUpVector(const glm::vec3 &up)
{
    sf::Listener::setUpVector(up.x, up.y, up.z);
}

glm::vec3 Listener::GetUpVector()
{
    auto pos = sf::Listener::getUpVector();
    return glm::vec3{pos.x, pos.y, pos.z};
}

void Listener::SetDirection(const glm::vec3 &dir)
{
    sf::Listener::setDirection(dir.x, dir.y, dir.z);
}

glm::vec3 Listener::GetDirection()
{
    auto pos = sf::Listener::getDirection();
    return glm::vec3{pos.x, pos.y, pos.z};
}

void Listener::SetVolume(float volume)
{
    sf::Listener::setGlobalVolume(volume * 100.f);
}

float Listener::GetVolume()
{
    return sf::Listener::getGlobalVolume() / 100.f;
}

} // end of namespace rl
