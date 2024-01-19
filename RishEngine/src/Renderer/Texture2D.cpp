#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/Image.h>
//
#include <Rish/Core/Log.h>
#include <Rish/Core/ResHolder.h>
#include <Rish/Core/VFS.h>
//
#include <glad/glad.h>

namespace rl {

Texture2DOption Texture2DOption::DefaultOption{};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Texture2D Constructors and Destructor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @brief  Constructor
* @details only create a texture without initializing it
*/
Texture2D::Texture2D()
{
    RL_PROFILE_FUNCTION();
    createTexture();
}

Texture2D::~Texture2D()
{
    RL_PROFILE_FUNCTION();
	glDeleteTextures(1, &m_textureID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    // [TODO] OpenGL 4.5
//    glTextureSubImage2D(m_textureID, 0, x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &p);
    // 4.1
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &p);
    //
}

void Texture2D::bind(uint32_t slot) const
{
    RL_PROFILE_FUNCTION();
    // [TODO] OpenGL 4.5
//	glBindTextureUnit(slot, m_textureID);

    // 4.1
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    //
}

void Texture2D::unbind() const
{
    RL_PROFILE_FUNCTION();

    // [TODO] OpenGL 4.5
//	glBindTextureUnit(0, 0);

    // 4.1
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    //
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static functions for Texture2D
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Ref<Texture2D> Texture2D::LoadTexture(const std::string &path, bool flip, Texture2DOption option)
{
    RL_PROFILE_FUNCTION();

    auto texture = MakeRef<Texture2D>();
    // Load image
    auto image = Image::LoadImage(path, flip);
    texture->setSize(image->getWidth(), image->getHeight());
    texture->setTexture(image->getPixelPtr());
    texture->m_path = path;
    texture->m_flip = flip;
    texture->setOptions(option);

    texture->m_init = true;

    return texture;
}

Ref<Texture2D> Texture2D::LoadTextureVFS(const std::string &virtualPath, bool flip, Texture2DOption option)
{
    RL_PROFILE_FUNCTION();

    auto texture = MakeRef<Texture2D>();
    std::string path;
    VFS::ResolvePhysicalPath(virtualPath, path);
    // Load image
    auto image = Image::LoadImage(path, flip);
    texture->setSize(image->getWidth(), image->getHeight());
    texture->setTexture(image->getPixelPtr());
    texture->m_path = path;
    texture->m_flip = flip;
    texture->setOptions(option);

    texture->m_init = true;

    return texture;
}

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, Texture2DOption option)
{
    auto texture = MakeRef<Texture2D>();

    texture->setSize(width, height);
    texture->m_path = "None";
    texture->m_init = true;
    texture->setOptions(option);

    return texture;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function of Texture2D
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Create the underlying texture with specified parameters
 * @param minFilter Method of texture filtering when minify
 * @param magFilter Method of texture filtering when magnify
 * @param wrapS Wrapping method of s-axis
 * @param wrapT Wrapping method of t-axis
 */
void Texture2D::createTexture()
{
    //[TODO] Old and New Version
    // OpenGL 4.5
//    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
    // OpenGL 4.1
    glGenTextures(1, &m_textureID);
    //
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    RL_ASSERT(m_textureID != 0, "[Texture2D] Failed to create a texture");
}

/**
 * @brief Set the Texture options
 * @param option Texture option
 */
void Texture2D::setOptions(const Texture2DOption &option)
{
    auto calculateFilterParam = [](Texture2DFilter filter) {
        if(filter & Texture2DFilter::TexNearest)
        {
            if(filter & Texture2DFilter::MipNearest)
                return GL_NEAREST_MIPMAP_NEAREST;
            else if(filter & Texture2DFilter::MipLinear)
                return GL_NEAREST_MIPMAP_LINEAR;
            else
                return GL_NEAREST;
        }
        else if(filter & Texture2DFilter::TexLinear)
        {
            if(filter & Texture2DFilter::MipNearest)
                return GL_LINEAR_MIPMAP_NEAREST;
            else if(filter & Texture2DFilter::MipLinear)
                return GL_LINEAR_MIPMAP_LINEAR;
            else
                return GL_LINEAR;
        }
        else
        {
            RL_CORE_ERROR("[Texture2D] No set any filter option. Rolled back to Texture2D::TexLinear");
            return GL_LINEAR;
        }
    };
    auto calculateWrapParam = [](Texture2DWrap wrap) {
        if(wrap == Texture2DWrap::Repeat)
            return GL_REPEAT;
        else if(wrap == Texture2DWrap::MirrorRepeat)
            return GL_MIRRORED_REPEAT;
        else if(wrap == Texture2DWrap::ClampToEdge)
            return GL_CLAMP_TO_EDGE;
        else if(wrap == Texture2DWrap::ClampToBorder)
            return GL_CLAMP_TO_BORDER;
        else
        {
            RL_CORE_ERROR("[Texture2D] No set any wrapping option. Rolled back to Texture2D::Repeat");
            return GL_REPEAT;
        }
    };
    //
    const Texture2DFilter &minFilter = option.minFilter;
    const Texture2DFilter &magFilter = option.magFilter;
    const Texture2DWrap &wrapS = option.wrapS;
    const Texture2DWrap &wrapT = option.wrapT;
    //
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    // [TODO] OpenGL 4.5
    //
//    glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, calculateFilterParam(minFilter));
//    glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, calculateFilterParam(magFilter));
//    //
//    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, calculateWrapParam(wrapS));
//    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, calculateWrapParam(wrapT));

    // OpenGL 4.1
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, calculateFilterParam(minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, calculateFilterParam(magFilter));
//
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, calculateWrapParam(wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, calculateWrapParam(wrapT));
    //
}

void Texture2D::setSize(uint32_t width, uint32_t height)
{
    // [TODO] OpenGL 4.5
//    glTextureStorage2D(m_textureID, 1, GL_RGBA8, width, height);
    // 4.1
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    //
    m_width = width;
    m_height = height;
}

void Texture2D::setTexture(const void *imagePtr)
{
    // [TODO] OpenGL 4.5
//    glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, imagePtr);
    // OpenGL 4.1
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, imagePtr);
    //
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