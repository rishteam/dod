#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/Image.h>
//
#include <Rish/Core/Log.h>
#include <Rish/Core/ResHolder.h>
#include <Rish/Core/VFS.h>
//
#include <glad/glad.h>

namespace rl {

Texture2D::Texture2D(const std::string &path, bool flip)
	: m_path(path)
{
	createTexture();

	auto image = Image::LoadImage(path, flip);

    setSize(image->getWidth(), image->getHeight());
    setTexture(image->getPixelPtr());
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &m_textureID);
}

Texture2D::Texture2D(Ref<Image> image)
{
    createTexture();
    setSize(image->getWidth(), image->getHeight());
    setTexture(image->getPixelPtr());
}

void Texture2D::bind(uint32_t slot) const
{
	glBindTextureUnit(slot, m_textureID);
}

void Texture2D::unbind() const
{
	glBindTextureUnit(0, 0);
}

Ref<Texture2D> Texture2D::LoadTexture(const std::string &path, bool flip)
{
    return MakeRef<Texture2D>(path, flip);
}

Ref<Texture2D> Texture2D::LoadTextureVFS(const std::string &virtualPath, bool flip)
{
    std::string path;
    VFS::ResolvePhysicalPath(virtualPath, path);
    return MakeRef<Texture2D>(path, flip);
}

void Texture2D::createTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
    RL_ASSERT(m_textureID != 0, "[Texture2D] Failed to create a texture");
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    //
    glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::setSize(uint32_t width, uint32_t height)
{
    glTextureStorage2D(m_textureID, 1, GL_RGBA8, width, height);
    m_width = width;
    m_height = height;
}

void Texture2D::setTexture(const void * imagePtr)
{
    glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, imagePtr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

}