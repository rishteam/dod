#include <array>
#include <exception>

#include <resManager.h>
#include <core.h>
#include <log.h>

namespace rl{

// Implementations for static variables
S_RES_FIELD_IMPL(Image);
S_RES_FIELD_IMPL(Texture);
S_RES_FIELD_IMPL(Sound);
std::vector<std::unique_ptr<sf::SoundBuffer>> ResManager::SoundBufferVec;
S_RES_FIELD_IMPL(Music);

// Root path for loading the resources
std::string ResManager::rootPath = ".";
void ResManager::setRootPath(const std::string &path)
{
    ResManager::rootPath = path;
}
std::string ResManager::getRootPath()
{
    return ResManager::rootPath;
}

std::string ResManager::getPrefix()
{
    const auto &last = ResManager::rootPath.back();
    if (last != '/' && last != '\\')
        ResManager::rootPath += '/';
    return ResManager::rootPath;
}

// help functions
std::ostream &operator<<(std::ostream &os, ResType type)
{
    std::array<std::string, ResCount> ResTypeName = {
        "ResImage",
        "ResTexture",
        "ResSound",
        "ResMusic"};
    return os << ResTypeName.at(static_cast<size_t>(type));
}

//
bool ResManager::loadRes(ResType type, std::string name, std::string path)
{
    path = ResManager::getPrefix() + path;
    bool res = false;
    switch(type)
    {
    case ResType::ResImage:
    case ResType::ResTexture:
    {
        if(path.find(".png") == std::string::npos &&
           path.find(".jpg") == std::string::npos)
        {
            RL_WARN("Wrong extention for image file : {}", path);
            return false;
        }

        if(type == ResType::ResImage)
        {
            auto & imageIdx = ResManager::ImageIdx;
            auto & imageVec = ResManager::ImageVec;
            if (!imageIdx.count(name))
            {
                imageIdx[name] = imageVec.size();
                imageVec.push_back(std::make_unique<sf::Image>());
            }
            sf::Image &image = *imageVec[imageIdx[name]];
            res = image.loadFromFile(path);
            if(!res)
            {
                RL_ERROR("Failed to load image: {} path={}", name, path);
                imageIdx.erase(name);
                imageVec.pop_back();
            }
        }
        else if(type == ResType::ResTexture)
        {
            auto &textureIdx = ResManager::TextureIdx;
            auto &textureVec = ResManager::TextureVec;
            if (!textureIdx.count(name))
            {
                textureIdx[name] = textureVec.size();
                textureVec.push_back(std::make_unique<sf::Texture>());
            }
            sf::Texture &tex = *textureVec[textureIdx[name]];
            res = tex.loadFromFile(path);
            if (!res)
            {
                RL_ERROR("Failed to load texture: {} path={}", name, path);
                textureIdx.erase(name);
                textureVec.pop_back();
            }
        }
    }
    break;

    case ResType::ResSound:
    case ResType::ResMusic:
    {
        if (path.find(".wav") == std::string::npos &&
            path.find(".ogg") == std::string::npos &&
            path.find(".flac") == std::string::npos)
        {
            RL_WARN("Wrong extention for sound file");
            return false;
        }

        if(type == ResType::ResSound)
        {
            auto &soundVec = ResManager::SoundVec;
            auto &soundBufVec = ResManager::SoundBufferVec;
            auto &soundIdx = ResManager::SoundIdx;
            if(!soundIdx.count(name))
            {
                soundIdx[name] = soundVec.size();
                soundVec.push_back(std::make_unique<sf::Sound>());
                soundBufVec.push_back(std::make_unique<sf::SoundBuffer>());
            }
            int idx = soundIdx[name];
            sf::SoundBuffer &soundBuf = *soundBufVec[idx];
            sf::Sound &sound = *soundVec[idx];

            res = soundBuf.loadFromFile(path);
            if (!res)
            {
                RL_ERROR("Failed to load sound: {} path={}", name, path);
                soundVec.pop_back();
                soundBufVec.pop_back();
                soundIdx.erase(name);
            }
            else
                sound.setBuffer(soundBuf);
        }
        else if(type == ResType::ResMusic)
        {
            auto &musicIdx = ResManager::MusicIdx;
            auto &musicVec = ResManager::MusicVec;
            if (!musicIdx.count(name))
            {
                musicIdx[name] = musicVec.size();
                musicVec.push_back(std::make_unique<sf::Music>());
            }
            int idx = musicIdx[name];
            sf::Music &music = *musicVec[idx];
            res = music.openFromFile(path);
            if(!res)
            {
                RL_ERROR("Failed to load music: {} path={}", name, path);
                musicVec.pop_back();
                musicIdx.erase(name);
            }
        }
    }
    break;
    default:
        RL_ERROR("Unsupported type: {}", type);
    break;
    }
    // success to load the resource
    if(res)
    {
        // RL_DEBUG("Succeeded to load {}: {} path={}", type, name, path);
    }
    return res;
}

bool ResManager::releaseRes(ResType type, std::string name)
{
    if(type == ResType::ResImage)
    {
        auto &imageIdx = ResManager::ImageIdx;
        auto &imageVec = ResManager::ImageVec;
        if(imageIdx.count(name))
        {
            imageVec.erase(imageVec.begin() + imageIdx[name]);
            imageIdx.erase(name);
        }
    }
    else if(type == ResType::ResTexture)
    {
        auto &texIdx = ResManager::TextureIdx;
        auto &texVec = ResManager::TextureVec;
        if (texIdx.count(name))
        {
            texVec.erase(texVec.begin() + texIdx[name]);
            texIdx.erase(name);
        }
    }
    else if(type == ResType::ResSound)
    {
        auto &soundIdx = ResManager::SoundIdx;
        auto &soundVec = ResManager::SoundVec;
        if (soundIdx.count(name))
        {
            soundVec.erase(soundVec.begin() + soundIdx[name]);
            soundIdx.erase(name);
        }
    }
    else if(type == ResType::ResMusic)
    {
        auto &musicIdx = ResManager::MusicIdx;
        auto &musicVec = ResManager::MusicVec;
        if (musicIdx.count(name))
        {
            musicVec.erase(musicVec.begin() + musicIdx[name]);
            musicIdx.erase(name);
        }
    }

    return true; // TODO: deal with false state
}

sf::Image& ResManager::getImage(std::string name)
{
    auto &imageIdx = ResManager::ImageIdx;
    auto &imageVec = ResManager::ImageVec;
    RL_ASSERT(imageIdx.count(name), "Image {} not found", name);
    return *imageVec[imageIdx[name]];
}

sf::Texture& ResManager::getTexture(std::string name)
{
    auto &texIdx = ResManager::TextureIdx;
    auto &texVec = ResManager::TextureVec;
    RL_ASSERT(texIdx.count(name), "Texture {} not found", name);
    return *texVec[texIdx[name]];
}

sf::Sound& ResManager::getSound(std::string name)
{
    auto &soundIdx = ResManager::SoundIdx;
    auto &soundVec = ResManager::SoundVec;
    RL_ASSERT(soundIdx.count(name), "Sound {} not found", name);
    return *soundVec[soundIdx[name]];
}

sf::Music& ResManager::getMusic(std::string name)
{
    auto &musicIdx = ResManager::MusicIdx;
    auto &musicVec = ResManager::MusicVec;
    RL_ASSERT(musicIdx.count(name), "Music {} not found", name);
    return *musicVec[musicIdx[name]];
}

}
