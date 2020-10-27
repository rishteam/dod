#include <Rish/Renderer/SubTexture2D.h>

namespace rl {

SubTexture2D::SubTexture2D(const rl::Ref<rl::Texture2D> &texture, const glm::vec2 &bottomLeft,
                           const glm::vec2 &topRight)
   : m_texture(texture)
{
    m_texCoords[0] = {bottomLeft.x ,bottomLeft.y};
    m_texCoords[1] = {topRight.x ,bottomLeft.y};
    m_texCoords[2] = {bottomLeft.x ,topRight.y};
    m_texCoords[3] = {topRight.x ,topRight.y};
}

Ref <SubTexture2D> SubTexture2D::CreateFromSheet(const Ref <Texture2D> &texture, const glm::vec2 pos, const glm::vec2 cellSize, const glm::vec2 spriteGridSize)
{
    float x = pos.x, y = pos.y;
    glm::vec2 texCoords[2] = {
        {x,                    y},                   // bottom left
        {x + spriteGridSize.x, y + spriteGridSize.y} // top right
    };

    for(auto & texCoord : texCoords)
    {
        texCoord.x = (texCoord.x * cellSize.x) / (float)texture->getWidth();
        texCoord.y = (texCoord.y * cellSize.y) / (float)texture->getHeight();
    }

    return MakeRef<SubTexture2D>(texture, texCoords[0], texCoords[1]);
}

Ref <SubTexture2D> SubTexture2D::CreateFromCoordinate(const Ref<Texture2D> &texture, const glm::vec2 &leftUpper, const glm::vec2 &size)
{
    float x = leftUpper.x;
    float y = leftUpper.y;

    glm::vec2 texCoords[2] = {
        {x, y - size.y},
        {x + size.x, y}
    };

    for(auto & texCoord : texCoords)
    {
        texCoord.x = texCoord.x / (float)texture->getWidth();
        texCoord.y = texCoord.y / (float)texture->getHeight();
    }

    return MakeRef<SubTexture2D>(texture, texCoords[0], texCoords[1]);
}

}
