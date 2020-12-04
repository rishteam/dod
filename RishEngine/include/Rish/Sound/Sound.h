#pragma once
#ifndef RISHENGINE_SOUND_H
#define RISHENGINE_SOUND_H

#include <Rish/rlpch.h>

namespace rl {

// fwd
class Time;

struct RL_API Sound3DSetting
{
    glm::vec3 position{0.f};
    float minDistance = 5.f;
    float attenuation = 1.f;
};

class RL_API Sound
{
public:
    static Ref<Sound> LoadSound(const std::string &path, const Sound3DSetting &setting={});
    static Ref<Sound> LoadSoundVFS(const std::string &path, const Sound3DSetting &setting={});

    Sound() = default;
    virtual ~Sound() = default;

    virtual void play()  = 0;
    virtual void pause() = 0;
    virtual void stop()  = 0;

    virtual bool isPlaying() = 0;
    virtual bool isStopped() = 0;
    virtual bool isPaused()  = 0;

    virtual void setLoop(bool loop) = 0;
    virtual bool getLoop()          = 0;

    virtual void setVolume(float volume) = 0;
    virtual float getVolume() = 0;

    virtual void setPlayingOffset(float offset) = 0;
    virtual Time getPlayingOffset()             = 0;
    virtual Time getLength() = 0;

    virtual void set3DSetting(const Sound3DSetting &setting) = 0;
    virtual Sound3DSetting get3DSetting() = 0;
};

} // end of namespace rl

#endif // RISHENGINE_SOUND_H
