#include <glad/glad.h>

#include <Rish/Renderer/VertexArray.h>

namespace rl {

/**
 * @brief Convert ShaderDataType to OpenGL enum
 * @param type Shader Data Type
 * @return GLenum
 */
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
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

void VertexArray::setVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	glBindVertexArray(vao);
	vertexBuffer->bind();
	RL_CORE_ASSERT(vertexBuffer->getLayout().getElements().size() > 0, "Vertex Buffer has no layout!");

	const auto &layout = vertexBuffer->getLayout();
	for(const auto & element: layout)
	{
		glEnableVertexAttribArray(m_vertexAttribIndex);

		glVertexAttribPointer(m_vertexAttribIndex,
            element.getComponentCount(),                  // Number of Components
            ShaderDataTypeToOpenGLBaseType(element.type), // Data type
			element.normalized ? GL_TRUE : GL_FALSE,      // Normalize
			layout.getStride(),                           // Stride
			(const void*)element.offset);                 // Offset
		m_vertexAttribIndex++;

	}
    m_vertexBuffer = vertexBuffer;
}

void VertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuf)
{
	glBindVertexArray(vao);
	indexBuf->bind();
	m_indexBuffer = indexBuf;
}

Ref<VertexArray> VertexArray::Create()
{
    return MakeRef<VertexArray>();
}

}