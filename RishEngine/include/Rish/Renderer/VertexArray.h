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

	static Ref<VertexArray> Create();

	void bind() const;
	void unbind() const;

	void setVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
	void setIndexBuffer(const Ref<IndexBuffer>& indexBuf);

	const Ref<VertexBuffer>& getVertexBuffer() const { return m_vertexBuffer; }
	const Ref<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

private:
	uint32_t vao = 0;
	/// vertex buffer
	Ref<VertexBuffer> m_vertexBuffer = nullptr;
	/// index buffer
	Ref<IndexBuffer> m_indexBuffer = nullptr;

	uint32_t m_vertexAttribIndex = 0;
};

}

/**
 * @class rl::VertexArray
 * How to create a VertexArray? First create a VertexBuffer and the BufferLayout. Remember to set the layout to the vertex buffer.<br/>
 * And then create the IndexBuffer. Lastly set both vertex buffer and index buffer to the vertex array
 * @code{.cpp}
 * rl::Ref<rl::VertexArray> vertexArray = rl::VertexArray::Create();
 *
 * float vertices[] = {
 *     //   a_Position      a_TexCord
 *     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
 *      0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
 *      0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
 *     -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
 * };
 *
 * // Vertex Buffer
 * rl::Ref<<rl::VertexBuffer> vb = rl::VertexBuffer::Create(vertices, sizeof(vertices));
 * // Create the buffer layout depending on the vertex attribute layout
 * rl::BufferLayout layout = {
 *     {rl::ShaderDataType::Float3, "a_Position"},
 *     {rl::ShaderDataType::Float2, "a_TexCord"}
 * };
 * vb->setLayout(layout); // Set the buffer layout to vertex buffer
 * vertexArray->setVertexBuffer(vb); // Set the vertex buffer to a vertex array
 *
 * // Index Buffer
 * uint32_t indices[] = {0, 1, 2, 2, 3, 0};
 * rl::Ref<rl::IndexBuffer> ib = rl::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
 * vertexArray->setIndexBuffer(ib); // Set the index/element buffer
 * @endcode
 */
