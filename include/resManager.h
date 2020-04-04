#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <fmt/printf.h>
#include <fmt/format.h>

namespace rl {

// macros for declaring and implementing the resource container
#define S_RES_FIELD(ID)                                                      \
    static std::unordered_map<std::string, std::unique_ptr<sf::ID>> ID ## Map

#define S_RES_FIELD_IMPL(ID)                                                     \
    std::unordered_map<std::string, std::unique_ptr<sf::ID>> ResManager::ID ## Map

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

class Animation; // forward declaration

// Resource Manager
class ResManager
{
public:
    static void init(const std::string &path);
    static void setRootPath(const std::string &path);
    static std::string getRootPath();
    static std::string getPrefix();   // with the slash
    // Load the resource
    static bool loadRes(ResType type, std::string name, std::string path);
    // Release the resource if it can be
    static bool releaseRes(ResType type, std::string name);
    // Get the file path of a resource
    static std::string getResPath(ResType type, std::string name);

    // Getters
    static sf::Image& getImage(std::string name);
    static sf::Texture& getTexture(std::string name);
    static sf::Sound& getSound(std::string name);
    static sf::Music& getMusic(std::string name);

private:
    S_RES_FIELD(Image);
    S_RES_FIELD(Texture);
    S_RES_FIELD(Sound);
    static std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> SoundBufferMap;
    S_RES_FIELD(Music);

public:
    // TODO: implement other missing resources
    static sf::Texture missingTexture;
    static Animation missingAnimation;

private:
    static std::string rootPath;
    static std::unordered_map<std::string, std::string> filePath[ResCount];
};

}