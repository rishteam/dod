#include <cstdio>
#include "config.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "core/resManager.h"

using namespace std;

int main()
{
    try
    {
        throw 1;
    }
    catch(int a)
    {
        printf("%d\n", a);
    }

    rl::ResManager<sf::Texture> texture;
    rl::ResManager<sf::Music> music;
}