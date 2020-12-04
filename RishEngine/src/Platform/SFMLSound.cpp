#include <Rish/rlpch.h>

#include <Rish/Platform/SFMLSound.h>
#include <Rish/Core/ResHolder.h>

namespace rl {

Ref<Sound> Sound::LoadSound(const std::string &path, const Sound3DSetting &setting)
{
    auto sound = MakeRef<SFMLSound>(path);
    sound->set3DSetting(setting);
    return sound;
}

Ref<Sound> Sound::LoadSoundVFS(const std::string &path, const Sound3DSetting &setting)
{
    std::string pp;
    VFS::ResolvePhysicalPath(path, pp);
    auto sound =  MakeRef<SFMLSound>(pp);
    sound->set3DSetting(setting);
    return sound;
}

SFMLSound::SFMLSound(const std::string &path)
{
    if(!ResHolder::Sound().exists(path))
        ResHolder::Sound().load(path, path);
    m_sound = &ResHolder::Sound().get(path);
}

void SFMLSound::play()
{
    m_sound->play();
}

void SFMLSound::pause()
{
    m_sound->pause();
}

void SFMLSound::stop()
{
    m_sound->stop();
}

bool SFMLSound::isPlaying()
{
    return m_sound->getStatus() == sf::Sound::Playing;
}

bool SFMLSound::isStopped()
{
    return m_sound->getStatus() == sf::Sound::Stopped;
}

bool SFMLSound::isPaused()
{
    return m_sound->getStatus() == sf::Sound::Paused;
}

void SFMLSound::setLoop(bool loop)
{
    m_sound->setLoop(loop);
}

bool SFMLSound::getLoop()
{
    return m_sound->getLoop();
}

void SFMLSound::setPlayingOffset(float offset)
{
    m_sound->setPlayingOffset(sf::seconds(offset));
}

Time SFMLSound::getPlayingOffset()
{
    return Time{m_sound->getPlayingOffset().asSeconds()};
}

void SFMLSound::set3DSetting(const Sound3DSetting &setting)
{
    m_sound->setPosition(setting.position.x, setting.position.y, setting.position.z);
    m_sound->setMinDistance(setting.minDistance);
    m_sound->setAttenuation(setting.attenuation);
}

Sound3DSetting SFMLSound::get3DSetting()
{
    return Sound3DSetting();
}

/**
 * Get the length of a sound
 * @return seconds
 */
Time SFMLSound::getLength()
{
    return Time{m_sound->getBuffer()->getDuration().asSeconds()};
}

void SFMLSound::setVolume(float volume)
{
    m_sound->setVolume(100.f * volume);
}

float SFMLSound::getVolume()
{
    return m_sound->getVolume() / 100.f;
}

} // end of namespace rl