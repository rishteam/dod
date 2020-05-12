/**
 * @file resHolder.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Resource Holder
 * @version 0.1
 * @date 2020-05-12
 */
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "core/resManager.h"

namespace rl {

/**
 * @brief Resource Holder class
 */
class ResHolder
{
public:
    /**
     * @brief Get ResHolder object (單例)
     * @return ResHolder& object
     */
    static ResHolder& Get();

    /// Texture Manager
    ResManager<sf::Texture> texture;
    static inline ResManager<sf::Texture>& Texture() { return Get().texture; }
    /// Image Manager
    ResManager<sf::Image> image;
    static inline ResManager<sf::Image> &Image() { return Get().image; }
    /// Font Manager
    ResManager<sf::Font> font;
    static inline ResManager<sf::Font> &Font() { return Get().font; }
    /// Music Manager
    ResManager<sf::Music> music;
    static inline ResManager<sf::Music> &Music() { return Get().music; }
    /// Sound Manager
    ResManager<sf::Sound> sound;
    static inline ResManager<sf::Sound> &Sound() { return Get().sound; }

private:
    /**
     * @brief Construct a new Res Holder object
     * @warning Private
     */
    ResHolder();
};

} // end namespace rl
