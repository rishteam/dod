#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/Texture2D.h>

#include <glm/glm.hpp>

namespace rl {

/**
 * @brief 2D sub-texture
 */
class RL_API SubTexture2D
{
public:
    SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &bottomLeft, const glm::vec2 &topRight);

    const Ref<Texture2D> getTexture() const { return m_texture; }
    const glm::vec2* getTextureCoords() const { return m_texCoords; }

    static Ref<SubTexture2D> CreateFromSheet(const Ref<Texture2D> &texture, const glm::vec2 pos, const glm::vec2 cellSize, const glm::vec2 spriteGridSize={1.f, 1.f});
    static Ref<SubTexture2D> CreateFromCoordinate(const Ref<Texture2D> &texture, const glm::vec2 &leftUpper, const glm::vec2 &size);
private:
    Ref<Texture2D> m_texture;
    glm::vec2 m_texCoords[4];
    float m_width, m_height;
};

struct SubTexture2DSetting
{
    enum SubTextureType
    {
        SubTextureSheet,
        SubTextureCoordinate
    };
    SubTextureType type{};

    glm::vec2 pos;
    glm::vec2 cellSize;
    glm::vec2 spriteGridSize{1.f, 1.f};

    glm::vec2 leftUpper;
    glm::vec2 size;

    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(
            CEREAL_NVP(type),
            CEREAL_NVP(leftUpper),
            CEREAL_NVP(size),
            CEREAL_NVP(pos),
            CEREAL_NVP(cellSize),
            CEREAL_NVP(spriteGridSize)
        );
    }
};

}
