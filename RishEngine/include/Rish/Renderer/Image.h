#pragma once

#include <Rish/rlpch.h>

namespace rl {

// TODO: make more functionalities for Image in the future ref: sf::Image

/**
 * @brief Image class
 */
class RL_API Image
{
public:
    virtual ~Image() = default;

    virtual const void* getPixelPtr() = 0;

    static Ref<Image> LoadImage(const std::string &path, bool flip=false);
    static Ref<Image> LoadImageVFS(const std::string &path, bool flip=false);

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    std::pair<uint32_t , uint32_t > getSize() const { return std::make_pair(m_width, m_height); }

protected:
    uint32_t m_width = 0, m_height = 0;
};

} // end of namespace rl
