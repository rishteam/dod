#pragma once
#ifndef RISHENGINE_SFMLSOUND_H
#define RISHENGINE_SFMLSOUND_H

#include <Rish/Sound/Sound.h>
#include <Rish/Core/Time.h>

// fwd
namespace sf {
class Sound;
}

namespace rl {

class SFMLSound : public Sound
{
public:
    SFMLSound(const std::string &path);
    virtual ~SFMLSound() override = default;

    void play() override;
    void pause() override;
    void stop() override;

    bool isPlaying() override;
    bool isStopped() override;
    bool isPaused() override;

    void setLoop(bool loop) override;
    bool getLoop() override;
    void setVolume(float volume) override;
    float getVolume() override;
    void setPlayingOffset(float offset) override;
    Time getPlayingOffset() override;
    Time getLength() override;
    void set3DSetting(const Sound3DSetting &setting) override;
    Sound3DSetting get3DSetting() override;

private:
    sf::Sound *m_sound = nullptr;
    Sound3DSetting m_setting;
};

} // end of namespace rl

#endif // RISHENGINE_SFMLSOUND_H