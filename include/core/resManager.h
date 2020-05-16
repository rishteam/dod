/**
 * @file resManager.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Resource Manager
 * @version 0.1
 * @date 2020-05-11
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "core/core.h"
#include "core/log.h"

namespace rl {

/**
 * @brief Resource Manager
 * @tparam T Type
 */
template <typename T>
class ResManager
{
public:
    bool load(const std::string &resName, const std::string &path);

    bool release(const std::string &resName);

    bool exists(const std::string &resName);

    std::string getResPath(const std::string &resName);

    T& get(const std::string &resName);

    T& getDefaultResource();
    bool isDefaultResource(const T &res) { return getDefaultResource() == res; }

private:
    inline void deleteIfExists(const std::string &resName, const std::string &path)
    {
        if (m_resMap.count(resName))
            m_resMap.erase(resName);
        if (m_resPath.count(path))
            m_resPath.erase(resName);
    }
    inline void setResourcePath(const std::string &resName, const std::string &path)
    {
        m_resPath[resName] = path;
    }

    /** @brief Mapping from strings to resources */
    std::unordered_map<std::string, T> m_resMap;

    /** @brief Mapping from resource names to resource paths */
    std::unordered_map<std::string, std::string> m_resPath;
};

/**
 * @brief Load a resource
 * @warning Plesase notice the loading resource type
 *          Is the resource loaded successfully
 * @note Suitable for `sf::Image`, `sf::Texture`, and `
 * @tparam T type
 * @param resName resource name
 * @param path resource path
 * @return true Succeeded
 * @return false Failed
 */
template <typename T>
bool ResManager<T>::load(const std::string &resName, const std::string &path)
{
    RL_TRACE("Load resource: {} {}", resName, path);

    // Delete if exists
    deleteIfExists(resName, path);

    // Create the resource
    auto &res = m_resMap[resName];
    // If load res failed
    if(!res.loadFromFile(path))
    {
        RL_ERROR("Failed to load res: {} {}", resName, path);
        m_resMap.erase(resName);
        return false;
    }
    setResourcePath(resName, path);
    return true;
}

/**
 * @brief Release a source by name.
 *
 * @tparam T Type
 * @param resName resource name
 * @param path resource path
 * @return true Succeeded
 * @return false Failed
 */
template <typename T>
bool ResManager<T>::release(const std::string &resName)
{
    int deleted = 0;
    if(m_resMap.count(resName))
        m_resMap.erase(resName), deleted++;
    if(m_resPath.count(resName))
        m_resPath.erase(resName), deleted++;
    return deleted == 2;
}

/**
 * @brief Check if resource exists
 *
 * @param resName resource name
 * @return true exist
 * @return false not exist
 */
template <typename T>
bool ResManager<T>::exists(const std::string &resName)
{
    return m_resMap.count(resName);
}

/**
 * @brief Get resource by name
 *
 * @tparam T Type
 * @param resName resource name
 * @return T& resource
 */
template <typename T>
T& ResManager<T>::get(const std::string &resName)
{
    if(!m_resMap.count(resName))
    {
        RL_WARN("Resource not found: {}", resName);
        return getDefaultResource();
    }
    return m_resMap[resName];
}

/**
 * @brief Get the file path of a resource
 * @warning return `Not found` if a resource name is not found in the path dict
 * @param resName resource name
 * @return std::string The path of the resource
 */
template <typename T>
std::string ResManager<T>::getResPath(const std::string &resName)
{
    return m_resPath.count(resName) ? m_resPath[resName] : "Not found";
}

/**
 * @brief Get the default resource
 * @tparam T Type
 * @return T& resource
 */
template <typename T>
T& ResManager<T>::getDefaultResource()
{
    static T defaultResource;
    return defaultResource;
}

/**
 * @brief Default `sf::Texture`
 *
 * @tparam
 * @return T& default texture
 */
template <>
inline sf::Texture &ResManager<sf::Texture>::getDefaultResource()
{
    RL_TRACE("load missing texture");
    if (!exists("default_texture"))
    {
        load("default_texture", "assets/missing_texture.png");
    }
    return get("default_texture");
}

/**
 * @brief Load `sf::Sound`
 *
 * @tparam
 * @param resName resource name
 * @param path resource path
 * @return true Succeeded
 * @return false Failed
 */
template <>
inline bool ResManager<sf::Sound>::load(const std::string &resName, const std::string &path)
{
    static std::unordered_map<std::string, sf::SoundBuffer> soundBufferMap;

    // delete if exists
    deleteIfExists(resName, path);
    //
    auto &sound = m_resMap[resName];
    // Load file into sound buffer
    auto &soundBuffer = soundBufferMap[resName];
    // if load res failed
    if (!soundBuffer.loadFromFile(path))
    {
        RL_ERROR("Failed to load res: {} {}", resName, path);
        m_resMap.erase(resName);
        return false;
    }
    sound.setBuffer(soundBuffer);
    // Set path
    setResourcePath(resName, path);
    return true;
}

/**
 * @brief
 *
 * @tparam
 * @param resName
 * @param path
 * @return true
 * @return false
 */
template <>
inline bool ResManager<sf::Music>::load(const std::string &resName, const std::string &path)
{
    deleteIfExists(resName, path);
    // Load the music
    auto &music = m_resMap[resName];
    if(!music.openFromFile(path))
    {
        RL_ERROR("Failed to load music: {} {}", resName, path);
        m_resMap.erase(resName);
        return false;
    }
    setResourcePath(resName, path);
    return true;
}

} // namespace rl