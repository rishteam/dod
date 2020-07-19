#include "Rish/Renderer/VertexArray.h"

namespace rl {

GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch(type)
	{
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
	}
	RL_CORE_ASSERT(false, "Unknown type");
	return 0;
}

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &vao);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &vao);
}

void VertexArray::bind() const
{
	glBindVertexArray(vao);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	glBindVertexArray(vao);
	vertexBuffer->bind();
	RL_CORE_ASSERT(vertexBuffer->getLayout().getElements().size() > 0, "Vertex Buffer has no layout!");

	const auto &layout = vertexBuffer->getLayout();
	for(const auto & element: layout)
	{
		glEnableVertexAttribArray(m_vertexBufferIndex);

		glVertexAttribPointer(m_vertexBufferIndex,
			element.getComponentCount(),
			ShaderDataTypeToOpenGLBaseType(element.type),
			element.normalized ? GL_TRUE : GL_FALSE,
			layout.getStride(),
			(const void*)element.offset
		);
		m_vertexBufferIndex++;

		m_vertexBuffer = vertexBuffer;
	}
}

void VertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuf)
{
	glBindVertexArray(vao);
	indexBuf->bind();
	m_indexBuffer = indexBuf;
}

}