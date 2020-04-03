#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <fmt/printf.h>
#include <fmt/format.h>

namespace rl {

// macros for declaring and implementing the resource container
#define S_RES_FIELD(ID)                                    \
    static std::unordered_map<std::string, int> ID ## Idx; \
    static std::vector<std::unique_ptr<sf::ID>> ID ## Vec

#define S_RES_FIELD_IMPL(ID)                                  \
    std::unordered_map<std::string, int> ResManager::ID ## Idx; \
    std::vector<std::unique_ptr<sf::ID>> ResManager::ID ## Vec

enum class ResType: char
{
    ResImage = 0,
    ResTexture,
    ResSound,
    ResMusic
};
#define ResCount 4
// for printing enum class ResType
std::ostream& operator<<(std::ostream& os, ResType type);

// Usage: e.g. load a texture (namesapce rl)
// ResManager::loadRes(ResTexture, "test", "/path/to/test.png");
//
// with copy and readonly
// sf::Texture tex = ResManager::getTexture("test");
//
// without copy and RW
// sf::Texture &tex = ResManager::getTexture("test");
//
// without copy and readonly
// const sf::Texture &tex = ResManager::getTexture("test");
//
// Set root dir of the resource path
// ResManager::setRootPath("C:/Users/Rish/Desktop/rish/dod");

// Resource Manager
class ResManager
{
public:
    static void setRootPath(const std::string &path);
    static std::string getRootPath();
    static std::string getPrefix();   // with the slash

    static bool loadRes(ResType type, std::string name, std::string path);
    static bool releaseRes(ResType type, std::string name);

    static sf::Image& getImage(std::string name);
    static sf::Texture& getTexture(std::string name);
    static sf::Sound& getSound(std::string name);
    static sf::Music& getMusic(std::string name);

private:
    S_RES_FIELD(Image);
    S_RES_FIELD(Texture);
    S_RES_FIELD(Sound);
    static std::vector<std::unique_ptr<sf::SoundBuffer>> SoundBufferVec;
    S_RES_FIELD(Music);

    static std::string rootPath;
};

}