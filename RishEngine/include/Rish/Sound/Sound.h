#pragma once
#ifndef RISHENGINE_SOUND_H
#define RISHENGINE_SOUND_H

#include <Rish/rlpch.h>

namespace rl {

class RL_API Sound
{
public:
    Sound() = default;
    virtual ~Sound() = default;

    static Ref<Sound> LoadSound(const std::string &path);
    static Ref<Sound> LoadSoundVFS(const std::string &path);
};

} // end of namespace rl

#endif // RISHENGINE_SOUND_H
