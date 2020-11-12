#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/VFS.h>
#include <Rish/Renderer/Image.h>

#include <SFML/Graphics/Image.hpp>

namespace rl {

class RL_API SFMLImage : public Image
{
public:
    SFMLImage(const std::string &pat, bool flip=true);
    ~SFMLImage();

    virtual const void* getPixelPtr() override;

private:
    sf::Image *m_image = nullptr; // TODO: refacter this to Ref when refactor the ResHolder
    std::string m_id;

    static std::unordered_map<std::string, std::string> s_pathToId;
};

} // end of namespace rl
