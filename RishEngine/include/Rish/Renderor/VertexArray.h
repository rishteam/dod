#include "Rish/rlpch.h"
#include "Rish/Core/Log.h"
#include "Rish/Renderor/Buffer.h"

#include <glad/glad.h>
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

	void addVertexBuffer(VertexBuffer *vertexBuf);
	void setIndexBufer(IndexBuffer *indexBuf);

private:
	uint32_t vao;
	VertexBuffer *m_vertexBuffer = nullptr;
	IndexBuffer *m_indexBuffer = nullptr;
	uint32_t m_vertexBufferIndex = 0;
};

}




