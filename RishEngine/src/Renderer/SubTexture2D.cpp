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

Ref <SubTexture2D> SubTexture2D::CreateFromSheet(const Ref <Texture2D> &texture, const glm::vec2 pos, const glm::vec2 cellSize, const glm::vec2 spriteSize)
{
    float x = pos.x, y = pos.y;
    float spriteW = cellSize.x, spriteH = cellSize.y;
    float W = (float)texture->getWidth(), H = (float)texture->getHeight();
    glm::vec2 texCoords[2] = {
        {(x*spriteW)/W, (y*spriteH)/H},
        {((x+spriteSize.x)*spriteW)/W, ((y+spriteSize.y)*spriteH)/H}
    };
    return MakeRef<SubTexture2D>(texture, texCoords[0], texCoords[1]);
}

}
