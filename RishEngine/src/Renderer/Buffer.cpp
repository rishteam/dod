#include <Rish/Renderer/Buffer.h>
#include <glad/glad.h>

namespace rl {

VertexBuffer::VertexBuffer()
{
    // [TODO] OpenGL 4.5
    //glCreateBuffers(1, &vbo);
    // 4.1
    glGenBuffers(1, &vbo);
    //
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

VertexBuffer::VertexBuffer(uint32_t size)
{
    // [TODO] OpenGL 4.5
//	glCreateBuffers(1, &vbo);
    // 4.1
    glGenBuffers(1, &vbo);
    //
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(float *vertices, uint32_t size)
{
    // [TODO] OpenGL 4.5
//	glCreateBuffers(1, &vbo);
    // 4.1
    glGenBuffers(1, &vbo);
    //
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &vbo);
}

void VertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void VertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setData(const void *data, uint32_t size)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

Ref<VertexBuffer> VertexBuffer::Create(float *vertices, uint32_t size)
{
    return MakeRef<VertexBuffer>(vertices, size);
}

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
    return MakeRef<VertexBuffer>(size);
}

Ref<VertexBuffer> VertexBuffer::Create()
{
    return rl::Ref<VertexBuffer>();
}

/////////////////////////////////////////////////////////////////////////////////////
// Index Buffer

IndexBuffer::IndexBuffer(uint32_t *indices, uint32_t count)
	: m_count(count)
{
    // [TODO] OpenGL 4.5
//	glCreateBuffers(1, &ebo);
    // 4.1
    glGenBuffers(1, &ebo);
    //
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &ebo);
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void IndexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t *indices, uint32_t count)
{
    return MakeRef<IndexBuffer>(indices, count);
}

}



