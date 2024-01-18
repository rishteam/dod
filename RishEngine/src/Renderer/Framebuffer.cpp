#include <glad/glad.h>
#include <Rish/Renderer/Framebuffer.h>


namespace rl {

Framebuffer::Framebuffer(const FramebufferSpecification &spec)
	: m_spec(spec)
{
    RL_PROFILE_FUNCTION();

    invalidate();
}

Framebuffer::~Framebuffer()
{
    RL_PROFILE_FUNCTION();

    glDeleteFramebuffers(1, &m_frameBufferID);
	glDeleteTextures(1, &m_colorAttachment);
	glDeleteTextures(1, &m_depthAttachment);
}

void Framebuffer::invalidate()
{
    RL_PROFILE_FUNCTION();

    if(m_frameBufferID)
	{
		glDeleteFramebuffers(1, &m_frameBufferID);
		glDeleteTextures(1, &m_colorAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}
    // Generate a framebuffer
    // [TODO] OpenGL 4.5
//	glCreateFramebuffers(1, &m_frameBufferID);
    // 4.1
    glGenFramebuffers(1, &m_frameBufferID);
    //
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);

    // Generate color attachment
    //[TODO] OpenGL 4.5
//	glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
    // 4.1
    glGenTextures(1, &m_colorAttachment);
    //
	glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_spec.width, m_spec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); // specify settings of texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// future: gl4 API support
//	 glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_spec.width, m_spec.height);
	// glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach the m_colorAttachment to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

    // Generate depth attachment
    // [TODO] OpenGL 4.5
//	glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
    // 4.1
    glGenBuffers(1, &m_depthAttachment);
    //

	glBindTexture(GL_TEXTURE_2D, m_depthAttachment);

    // [TODO] OpenGL 4.5
//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_spec.width, m_spec.height);
    // 4.1 Not sure if this is equivalent
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_spec.width, m_spec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    //

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

	RL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind() const
{
    RL_PROFILE_FUNCTION();

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
	glViewport(0, 0, m_spec.width, m_spec.height);
}

void Framebuffer::unbind()
{
    RL_PROFILE_FUNCTION();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(uint32_t width, uint32_t height)
{
	m_spec.width = width;
	m_spec.height = height;

	invalidate();
}

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
{
    return MakeRef<Framebuffer>(spec);
}

}
