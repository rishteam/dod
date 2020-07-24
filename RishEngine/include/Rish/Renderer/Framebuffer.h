/**
 * @file Framebuffer.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Header of framebuffer
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

namespace rl {

/**
 * @brief Framebuffer Settings
 */
struct FramebufferSpecification
{
	uint32_t width{};
	uint32_t height{};
	uint32_t sample = 1;
};

/**
 * @brief Frame Buffer Object
 */
class RL_API Framebuffer
{
public:
    /**
     * @brief ctor
     * @param spec Framebuffer Settings
     */
	Framebuffer(const FramebufferSpecification &spec);

	/**
	 * @brief dtor
	 */
	~Framebuffer();

	/**
	 * @brief Recreate the framebuffer using the specification
	 */
	void invalidate();

	void bind();
	void unbind();

	void resize(uint32_t width, uint32_t height);

	uint32_t getColorAttachmentRendererID() const { return m_colorAttachment; }
	const FramebufferSpecification& getSpecification() const { return m_spec; }

private:
	uint32_t m_frameBufferID = 0;
	uint32_t m_colorAttachment = 0;
	uint32_t m_depthAttachment = 0;
	FramebufferSpecification m_spec;
};

}
