#pragma once
#ifndef RISHENGINE_SOUND_COMPONENT_H
#define RISHENGINE_SOUND_COMPONENT_H

#include <Rish/Sound/Sound.h>

namespace rl {

class RL_API SoundComponent
{
public:

private:
    Ref<Sound> m_sound;

    friend class cereal::access;
    template<class Archive>
    void serialize(Archive &ar) const
    {
        ar(cereal::make_nvp("sound", m_sound));
    }
};

} // end of namespace rl

#endif // RISHENGINE_SOUND_COMPONENT_H
