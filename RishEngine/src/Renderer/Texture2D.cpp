#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/Image.h>
//
#include <Rish/Core/Log.h>
#include <Rish/Core/ResHolder.h>
#include <Rish/Core/VFS.h>
//
#include <glad/glad.h>

namespace rl {

Texture2D::Texture2D()
{
    createTexture();
}

Texture2D::Texture2D(uint32_t width, uint32_t height)
{
    RL_PROFILE_FUNCTION();

    createTexture();
    setSize(width, height);
    m_path = "None";
}

Texture2D::Texture2D(const std::string &path, bool flip)
{
    RL_PROFILE_FUNCTION();

	createTexture();

	auto image = Image::LoadImage(path, flip);

    setSize(image->getWidth(), image->getHeight());
    setTexture(image->getPixelPtr());
    m_path = path;
    m_flip = flip;
}

Texture2D::~Texture2D()
{
    RL_PROFILE_FUNCTION();

	glDeleteTextures(1, &m_textureID);
}

////////////////////////////////////////////////////

void Texture2D::setData(void *data, uint32_t size)
{
    RL_PROFILE_FUNCTION();

    RL_ASSERT(size == m_width * m_height * 4, "data must be entire texture");
    glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void Texture2D::setPixel(uint32_t x, uint32_t y, const glm::vec4 &color)
{
    RL_PROFILE_FUNCTION();

    RL_ASSERT(x <= m_width && y <= m_height, "Pixel must inside the texture");

    union Pixel
    {
        struct pixel
        {
            uint8_t r, g, b, a;
        }p;
        uint32_t data;
    };
    Pixel p{};
    p.p.r = (uint8_t)roundf(color.r * 0xff);
    p.p.g = (uint8_t)roundf(color.g * 0xff);
    p.p.b = (uint8_t)roundf(color.b * 0xff);
    p.p.a = (uint8_t)roundf(color.a * 0xff);
    glTextureSubImage2D(m_textureID, 0, x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &p);
}

void Texture2D::bind(uint32_t slot) const
{
    RL_PROFILE_FUNCTION();

	glBindTextureUnit(slot, m_textureID);
}

void Texture2D::unbind() const
{
    RL_PROFILE_FUNCTION();

	glBindTextureUnit(0, 0);
}

////////////////////////////////////////////////////
// static function for Texture2D

Ref<Texture2D> Texture2D::LoadTexture(const std::string &path, bool flip)
{
    RL_PROFILE_FUNCTION();

    return MakeRef<Texture2D>(path, flip);
}

Ref<Texture2D> Texture2D::LoadTextureVFS(const std::string &virtualPath, bool flip)
{
    RL_PROFILE_FUNCTION();

    std::string path;
    VFS::ResolvePhysicalPath(virtualPath, path);
    return MakeRef<Texture2D>(path, flip);
}

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
    return MakeRef<Texture2D>(width, height);
}

////////////////////////////////////////////////////

void Texture2D::createTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
    RL_ASSERT(m_textureID != 0, "[Texture2D] Failed to create a texture");
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    //
    glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture2D::setSize(uint32_t width, uint32_t height)
{
    glTextureStorage2D(m_textureID, 1, GL_RGBA8, width, height);
    m_width = width;
    m_height = height;
}

void Texture2D::setTexture(const void *imagePtr)
{
    glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, imagePtr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::operator==(const Texture2D &rhs) const
{
    std::hash<Texture2D> hasher;
    return hasher(*this) == hasher(rhs);
}

bool Texture2D::operator!=(const Texture2D &rhs) const
{
    return !(rhs == *this);
}

}