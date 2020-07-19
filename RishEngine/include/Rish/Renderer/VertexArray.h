#pragma once

#include <glad/glad.h>
#include "Rish/rlpch.h"
#include "Rish/Core/Log.h"
#include "Rish/Renderer/Buffer.h"

#include <glm/glm.hpp>


namespace rl {

GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;

	void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
	void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuf);

	const std::shared_ptr<VertexBuffer>& getVertexBuffer() const { return m_vertexBuffer; }
	const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

private:
	uint32_t vao;
	std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
	std::shared_ptr<IndexBuffer> m_indexBuffer = nullptr;

	uint32_t m_vertexBufferIndex = 0;
};

}




