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
 * @brief
 */
class ResHolder
{
public:
    static ResHolder& Get();

    ResManager<sf::Texture> texture;
    static inline ResManager<sf::Texture>& Texture() { return Get().texture; }
    //
    ResManager<sf::Image> image;
    static inline ResManager<sf::Image> &Image() { return Get().image; }
    //
    ResManager<sf::Font> font;
    static inline ResManager<sf::Font> &Font() { return Get().font; }
    //
    ResManager<sf::Music> music;
    static inline ResManager<sf::Music> &Music() { return Get().music; }
    //
    ResManager<sf::Sound> sound;
    static inline ResManager<sf::Sound> &Sound() { return Get().sound; }

private:
    ResHolder();
};

} // end namespace rl
