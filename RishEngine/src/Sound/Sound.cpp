#include <Rish/rlpch.h>
#include <Rish/Sound/Sound.h>

#include <Rish/Core/VFS.h>
#include <Rish/Core/Time.h>
#include <Rish/Core/ResHolder.h>

namespace rl {

Ref<Sound> Sound::LoadSound(const std::string &path, const Sound3DSetting &setting)
{
    auto sound = MakeRef<Sound>(path);
    sound->set3DSetting(setting);
    return sound;
}

Ref<Sound> Sound::LoadSoundVFS(const std::string &path, const Sound3DSetting &setting)
{
    std::string pp;
    VFS::ResolvePhysicalPath(path, pp);
    auto sound = MakeRef<Sound>(pp);
    sound->set3DSetting(setting);
    return sound;
}

Sound::Sound()
{
}

Sound::Sound(const std::string &path)
{
    loadFromSoundFile(path);
    m_path = path;
}

void Sound::play()
{
    m_sound->play();
}

void Sound::pause()
{
    m_sound->pause();
}

void Sound::stop()
{
    m_sound->stop();
}

bool Sound::isPlaying()
{
    return m_sound->getStatus() == sf::Sound::Playing;
}

bool Sound::isStopped()
{
    return m_sound->getStatus() == sf::Sound::Stopped;
}

bool Sound::isPaused()
{
    return m_sound->getStatus() == sf::Sound::Paused;
}

void Sound::setLoop(bool loop)
{
    m_sound->setLoop(loop);
}

bool Sound::getLoop()
{
    return m_sound->getLoop();
}

void Sound::setPlayingOffset(float offset)
{
    m_sound->setPlayingOffset(sf::seconds(offset));
}

Time Sound::getPlayingOffset()
{
    return Time{m_sound->getPlayingOffset().asSeconds()};
}

void Sound::set3DSetting(const Sound3DSetting &setting)
{
    m_sound->setPosition(setting.position.x, setting.position.y, setting.position.z);
    m_sound->setMinDistance(setting.minDistance);
    m_sound->setAttenuation(setting.attenuation);
}

Sound3DSetting Sound::get3DSetting()
{
    return Sound3DSetting();
}

/**
 * Get the length of a sound
 * @return seconds
 */
Time Sound::getLength()
{
    return Time{m_sound->getBuffer()->getDuration().asSeconds()};
}

void Sound::setVolume(float volume)
{
    m_sound->setVolume(100.f * volume);
}

float Sound::getVolume()
{
    return m_sound->getVolume() / 100.f;
}

void Sound::loadFromSoundFile(const std::string &path)
{
    if(!ResHolder::Sound().exists(path))
        ResHolder::Sound().load(path, path);
    m_sound = &ResHolder::Sound().get(path);
}

} // end of namespace rl