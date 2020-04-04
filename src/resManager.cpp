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
std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> ResManager::SoundBufferMap;
S_RES_FIELD_IMPL(Music);

std::unordered_map<std::string, std::string> ResManager::filePath[ResCount];

// Init
void ResManager::init(const std::string &path)
{
    ResManager::setRootPath(path);
    // Load missings
    ResManager::loadRes(ResType::ResTexture, "missing_texture", "./assets/missing_texture.png");
    ResManager::missingTexture = &ResManager::getTexture("missing_texture");
}

sf::Texture *ResManager::missingTexture = nullptr;

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

std::string ResManager::getResPath(ResType type, std::string name)
{
    const int which = static_cast<size_t>(type);
    if(filePath[which].count(name))
    {
        // Remove the rootPath (get relative path of the resource)
        std::string path = filePath[which][name];
        size_t rLen = ResManager::rootPath.size();
        size_t i = -1;
        if((i=path.find(ResManager::rootPath)) != std::string::npos)
            path = path.substr(rLen, path.size()-rLen);
        return path;
    }
    else
        return "";
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
            auto &imageMap = ResManager::ImageMap;
            if (!imageMap.count(name))
            {
                imageMap[name] = std::make_unique<sf::Image>();
            }
            sf::Image &image = *imageMap[name];
            res = image.loadFromFile(path);
            if(!res)
            {
                RL_ERROR("Failed to load image: {} path={}", name, path);
                imageMap.erase(name);
            }
        }
        else if(type == ResType::ResTexture)
        {
            auto &textureMap = ResManager::TextureMap;
            if (!textureMap.count(name))
            {
                textureMap[name] = std::make_unique<sf::Texture>();
            }
            sf::Texture &tex = *textureMap[name];
            res = tex.loadFromFile(path);
            if (!res)
            {
                RL_ERROR("Failed to load texture: {} path={}", name, path);
                textureMap.erase(name);
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
            auto &soundMap = ResManager::SoundMap;
            auto &soundBufferMap = ResManager::SoundBufferMap;
            if(!soundMap.count(name))
            {
                soundMap[name] = std::make_unique<sf::Sound>();
            }
            if(!soundBufferMap.count(name))
                soundBufferMap[name] = std::make_unique<sf::SoundBuffer>();
            sf::SoundBuffer &soundBuf = *soundBufferMap[name];
            sf::Sound &sound = *soundMap[name];

            res = soundBuf.loadFromFile(path);
            if (!res)
            {
                RL_ERROR("Failed to load sound: {} path={}", name, path);
                soundMap.erase(name);
                soundBufferMap.erase(name);
            }
            else
                sound.setBuffer(soundBuf);
        }
        else if(type == ResType::ResMusic)
        {
            auto &musicMap = ResManager::MusicMap;
            if (!musicMap.count(name))
                musicMap[name] = std::make_unique<sf::Music>();
            sf::Music &music = *musicMap[name];
            res = music.openFromFile(path);
            if(!res)
            {
                RL_ERROR("Failed to load music: {} path={}", name, path);
                musicMap.erase(name);
            }
        }
    }
    break;
    default:
        res = false;
        RL_ERROR("Unsupported type: {}", type);
    break;
    }
    // success to load the resource
    if(res)
    {
        const int which = static_cast<int>(type);
        filePath[which][name] = path;
        // RL_DEBUG("Succeeded to load {}: {} path={}", type, name, path);
    }
    return res;
}

bool ResManager::releaseRes(ResType type, std::string name)
{
    if(type == ResType::ResImage)
    {
        auto &imageMap = ResManager::ImageMap;
        if(imageMap.count(name))
            imageMap.erase(name);
    }
    else if(type == ResType::ResTexture)
    {
        auto &textureMap = ResManager::TextureMap;
        if (textureMap.count(name))
            textureMap.erase(name);
    }
    else if(type == ResType::ResSound)
    {
        auto &soundMap = ResManager::SoundMap;
        if (soundMap.count(name))
            soundMap.erase(name);
        auto &soundBufferMap = ResManager::SoundBufferMap;
        if (soundBufferMap.count(name))
            soundBufferMap.erase(name);
    }
    else if(type == ResType::ResMusic)
    {
        auto &musicMap = ResManager::MusicMap;
        if (musicMap.count(name))
            musicMap.erase(name);
    }
    const int which = static_cast<int>(type);
    if(filePath[which].count(name))
        filePath[which].erase(name);

    return true; // TODO: deal with false state
}

sf::Image& ResManager::getImage(std::string name)
{
    auto &imageMap = ResManager::ImageMap;
    RL_ASSERT(imageMap.count(name), "Image {} not found", name);
    return *imageMap[name];
}

sf::Texture& ResManager::getTexture(std::string name)
{
    auto &texMap = ResManager::TextureMap;
    if(!texMap.count(name))
    {
        RL_WARN("Missing texture: {}\n", name);
        return *ResManager::missingTexture;
    }
    else
        return *texMap[name];
}

sf::Sound& ResManager::getSound(std::string name)
{
    auto &soundMap = ResManager::SoundMap;
    RL_ASSERT(soundMap.count(name), "Sound {} not found", name);
    return *soundMap[name];
}

sf::Music& ResManager::getMusic(std::string name)
{
    auto &musicMap = ResManager::MusicMap;
    RL_ASSERT(musicMap.count(name), "Music {} not found", name);
    return *musicMap[name];
}

}
