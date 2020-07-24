#include <Rish/Renderer/Texture2D.h>
#include <Rish/Core/Log.h>
#include <Rish/Core/ResHolder.h>
//
#include <fmt/format.h>
#include <glad/glad.h>

namespace rl {

Texture2D::Texture2D(const std::string& path)
	: m_path(path)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
	RL_ASSERT(m_textureID != 0, "Failed to create a texture");
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	auto texName = fmt::format("texture_{}", m_textureID);
    if(!ResHolder::Image().load(texName, path))
    {
        RL_ERROR("Failed to load texture {} from {}", texName, path);
    }
    auto &image = ResHolder::Image().get(texName);

	// TODO maybe ImGui::Image's (0, 0) pos is different from openGL
	// image.flipVertically();

	m_width = image.getSize().x;
	m_height = image.getSize().y;

	glTextureStorage2D(m_textureID, 1, GL_RGB8, m_width, m_height);

	glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)image.getPixelsPtr());
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &m_textureID);
}

void Texture2D::bind(uint32_t slot) const
{
	glBindTextureUnit(slot, m_textureID);
}

void Texture2D::unbind() const
{
	glBindTextureUnit(0, 0);
}

}