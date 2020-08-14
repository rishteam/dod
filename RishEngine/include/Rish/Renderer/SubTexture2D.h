#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/Texture2D.h>

#include <glm/glm.hpp>

namespace rl {

class RL_API SubTexture2D
{
public:
    SubTexture2D(const Ref<Texture2D> &texture, const glm::vec2 &bottomLeft, const glm::vec2 &topRight);

    const Ref<Texture2D> getTexture() const { return m_texture; }
    const glm::vec2* getTextureCoords() const { return m_texCoords; }

    static Ref<SubTexture2D> CreateFromSheet(const Ref<Texture2D> &texture, const glm::vec2 pos, const glm::vec2 cellSize, const glm::vec2 spriteSize={1.f, 1.f});
private:
    Ref<Texture2D> m_texture;
    glm::vec2 m_texCoords[4];
    float m_width, m_height;
};

}
