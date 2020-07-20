#pragma once

#include "Rish/rlpch.h"

namespace rl {

struct FramebufferSpecification
{
	uint32_t width;
	uint32_t height;
	uint32_t sample = 1;
};

class Framebuffer
{
public:
	Framebuffer(const FramebufferSpecification &spec);
	~Framebuffer();

	void invalidate();// Recreate framebuffer

	void bind();
	void unbind();

	void resize(uint32_t width, uint32_t height);

	uint32_t getColorAttachmentRendererID() const { return m_colorAttachment; }
	const FramebufferSpecification& getSpecification() const { return m_spec; }
private:

	uint32_t m_rendererID = 0;
	uint32_t m_colorAttachment = 0;
	uint32_t m_depthAttachment = 0;
	FramebufferSpecification m_spec;
};


}
