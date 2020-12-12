#pragma once
#ifndef RISHENGINE_SOUND_H
#define RISHENGINE_SOUND_H

// fwd
namespace sf {
class Sound;
}

namespace rl {

// fwd
class Time;

struct RL_API Sound3DSetting
{
    glm::vec3 position{0.f};
    float minDistance = 5.f;
    float attenuation = 1.f;

    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(position));
        ar(CEREAL_NVP(minDistance));
        ar(CEREAL_NVP(attenuation));
    }
};

// 2020-12-06 roy4801
// The sound part of RishEngine is written without proper separation with the underlying library
// When the cereal library f**ked me with the polymorphism serialization, I could only jam in the code under the time pressure
// **Need rewrite in the future**

class RL_API Sound
{
public:
    static Ref<Sound> LoadSound(const std::string &path, const Sound3DSetting &setting={});
    static Ref<Sound> LoadSoundVFS(const std::string &path, const Sound3DSetting &setting={});

    Sound();
    Sound(const std::string &path);
    ~Sound() = default;

    void play();
    void pause();
    void stop();

    bool isPlaying();
    bool isStopped();
    bool isPaused();

    void setLoop(bool loop);
    bool getLoop();

    void setVolume(float volume);
    float getVolume();

    void setPlayingOffset(float offset);
    Time getPlayingOffset();
    Time getLength();

    void set3DSetting(const Sound3DSetting &setting);
    Sound3DSetting get3DSetting();

private:
    std::string m_path;
    void loadFromSoundFile(const std::string &path);

    sf::Sound *m_sound = nullptr;
    Sound3DSetting m_setting;

    friend class ComponentEditPanel;

    friend class cereal::access;
    template<class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::make_nvp("path", m_path));
        ar(cereal::make_nvp("setting", m_setting));
    }

    template<class Archive>
    void load(Archive &ar)
    {
        ar(cereal::make_nvp("path", m_path));
        ar(cereal::make_nvp("setting", m_setting));
        loadFromSoundFile(m_path);
    }
};

} // end of namespace rl

#endif // RISHENGINE_SOUND_H
