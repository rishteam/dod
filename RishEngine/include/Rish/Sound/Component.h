#pragma once
#ifndef RISHENGINE_SOUND_COMPONENT_H
#define RISHENGINE_SOUND_COMPONENT_H

#include <Rish/Sound/Sound.h>

namespace rl {

class RL_API SoundComponent
{
public:
    void loadSound(const std::string &path)
    {
        m_sound = Sound::LoadSound(FileSystem::RelativePath(path));
    }

//private:
    Ref<Sound> m_sound;
    float m_volume      = 1.f;
    bool m_loop         = false;
    bool m_enableSpatial = false;

    friend class cereal::access;
    template<class Archive>
    void serialize(Archive &ar) const
    {
        ar(cereal::make_nvp("sound", m_sound));
        ar(cereal::make_nvp("volume", m_volume));
        ar(cereal::make_nvp("loop", m_loop));
        ar(cereal::make_nvp("enableSpatial", m_enableSpatial));

        m_sound->setVolume(m_volume);
        m_sound->setLoop(m_loop);
    }
};

} // end of namespace rl

#endif // RISHENGINE_SOUND_COMPONENT_H
