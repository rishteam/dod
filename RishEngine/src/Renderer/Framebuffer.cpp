#include "Rish/Renderer/Framebuffer.h"
#include "Rish/Core/Log.h"
#include <glad/glad.h>


namespace rl {

Framebuffer::Framebuffer(const FramebufferSpecification &spec)
	: m_spec(spec)
{
	invalidate();
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_rendererID);
	glDeleteTextures(1, &m_colorAttachment);
	glDeleteTextures(1, &m_depthAttachment);
}

void Framebuffer::invalidate()
{
	if(m_rendererID)
	{
		glDeleteFramebuffers(1, &m_rendererID);
		glDeleteTextures(1, &m_colorAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}

	glCreateFramebuffers(1, &m_rendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
	glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_spec.width, m_spec.height, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, m_spec.width, m_spec.height);
	// glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
	glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_spec.width, m_spec.height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL, GL_TEXTURE_2D, m_depthAttachment, 0);

	RL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
	glViewport(0, 0, m_spec.width, m_spec.height);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(uint32_t width, uint32_t height)
{
	m_spec.width = width;
	m_spec.height = height;

	invalidate();
}

}
