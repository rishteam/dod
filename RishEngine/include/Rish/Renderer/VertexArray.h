/**
 * @file VertexArray.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Header of VertexArray
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <glm/glm.hpp>
//
#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/Buffer.h>

namespace rl {

/**
 * @brief Vertex Array Object
 * @details VertexArray contains a VertexBuffer and a IndexBuffer(Optional)
 */
class RL_API VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;

	void setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
	void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuf);

	const std::shared_ptr<VertexBuffer>& getVertexBuffer() const { return m_vertexBuffer; }
	const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

private:
	uint32_t vao = 0;
	/// vertex buffer
	std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
	/// index buffer
	std::shared_ptr<IndexBuffer> m_indexBuffer = nullptr;

	uint32_t m_vertexAttribIndex = 0;
};

}
