/**
 * @file Buffer.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Related to vertex attribute
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

namespace rl
{

/**
 * @brief Shader data type enum
 */
enum class ShaderDataType
{
	None = 0,  ///< None
	Float,     ///< float
	Float2,    ///< vec2
	Float3,    ///< vec3
	Float4,	   ///< vec4
	Mat3,      ///< mat3
	Mat4,      ///< mat4
	Int,       ///< int
	Int2,      ///< ivec2
	Int3,      ///< ivec3
	Int4,      ///< ive4
	Bool       ///< bool
};

/**
 * @brief Return size in bytes by its type
 * @param type Shader Data Type
 * @return Size in bytes
 */
static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:
		return 4;
	case ShaderDataType::Float2:
		return 4 * 2;
	case ShaderDataType::Float3:
		return 4 * 3;
	case ShaderDataType::Float4:
		return 4 * 4;
	case ShaderDataType::Mat3:
		return 4 * 3 * 3;
	case ShaderDataType::Mat4:
		return 4 * 4 * 4;
	case ShaderDataType::Int:
		return 4;
	case ShaderDataType::Int2:
		return 4 * 2;
	case ShaderDataType::Int3:
		return 4 * 4;
	case ShaderDataType::Int4:
		return 4 * 4;
	case ShaderDataType::Bool:
		return 1;

	case ShaderDataType::None:
	default:
		RL_CORE_ASSERT(false, "Unknown ShaderDataType");
	}
	return 0;
}

/**
 * @brief Vertex Attribute Element
 * @details The class defines a vertex attribute with name and its data type
 */
struct RL_API BufferElement
{
	std::string name;
	ShaderDataType type;
	uint32_t size;
	size_t offset;
	bool normalized;

	BufferElement() = default;
	BufferElement(ShaderDataType type_, const std::string &name_, bool normalized_ = false)
		: name(name_), type(type_), size(ShaderDataTypeSize(type_)), offset(0), normalized(normalized_)
	{
	}

    /**
     * @brief Get the component count
     * @return Component count
     */
	uint32_t getComponentCount() const
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 1;
		case ShaderDataType::Float2:
			return 2;
		case ShaderDataType::Float3:
			return 3;
		case ShaderDataType::Float4:
			return 4;
		case ShaderDataType::Mat3:
			return 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4;
		case ShaderDataType::Int:
			return 1;
		case ShaderDataType::Int2:
			return 2;
		case ShaderDataType::Int3:
			return 3;
		case ShaderDataType::Int4:
			return 4;
		case ShaderDataType::Bool:
			return 1;
		}
		RL_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}
};

/**
 * @brief Vertex Attribute Container
 * @details This class contains a series of BufferElement which defined by client. <br/>
 * It will calculate the stride and offset for the vertex attributes.
 */
class RL_API BufferLayout
{
public:
	BufferLayout() {}
	BufferLayout(const std::initializer_list<BufferElement> &elements)
		: m_elements(elements)
	{
		calculateOffsetAndStride();
	}

	uint32_t getStride() const { return m_stride; }
	const std::vector<BufferElement> &getElements() const { return m_elements; }

	using iterator = std::vector<BufferElement>::iterator;
	using const_iterator = std::vector<BufferElement>::const_iterator;

	iterator begin() { return m_elements.begin(); }
	iterator end() { return m_elements.end(); }
	const_iterator begin() const { return m_elements.begin(); }
	const_iterator end() const { return m_elements.end(); }

private:

	void calculateOffsetAndStride()
	{
		size_t offset = 0;
		m_stride = 0;
		for(auto &ele : m_elements)
		{
			ele.offset = offset;
			offset += ele.size;
			m_stride += ele.size;
		}
	}

	std::vector<BufferElement> m_elements;
	uint32_t m_stride = 0;
};

/**
 * @brief Vertex Buffer Object
 * @details It consists of a VBO id and a BufferLayout
 */
class RL_API VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(uint32_t size);
	VertexBuffer(float *vertices, uint32_t size);
	~VertexBuffer();

	void bind() const;
	void unbind() const;

	void setData(const void* data, uint32_t size);

	const BufferLayout& getLayout() const { return m_layout; }
	void setLayout(const BufferLayout& layout) { m_layout = layout; }

	static Ref<VertexBuffer> Create(float *vertices, uint32_t size);
	static Ref<VertexBuffer> Create(uint32_t size);
	static Ref<VertexBuffer> Create();

private:
	uint32_t vbo;
	BufferLayout m_layout;
};

/**
 * @brief Index/Element Buffer Object
 * @details It consists of a EBO id and count
 */
class RL_API IndexBuffer
{
public:
	IndexBuffer(uint32_t *indices, uint32_t count);
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	/**
	 * @brief Get number vertex in index buffer
	 * @return count
	 */
	uint32_t getCount() const { return m_count; }

	static Ref<IndexBuffer> Create(uint32_t *indices, uint32_t count);
private:
	uint32_t ebo;
	uint32_t m_count;
};

} // namespace rl

/**
 * @class rl::BufferLayout
 * Construct a buffer layout depending on the layout of your vertex data.
 *
 * @code{.cpp}
 * float vertices[] = {
 *      //   a_Position      a_TexCord
 *      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
 *       0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
 *       0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
 *      -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
 *  };
 * rl::BufferLayout layout = {
 *     {rl::ShaderDataType::Float3, "a_Position"},
 *     {rl::ShaderDataType::Float2, "a_TexCord"}
 * };
 * @endcode
 * @code{.glsl}
 * // In Shader
 * layout(location = 0) in vec3 a_Position;
 * layout(location = 1) in vec2 a_TexCord;
 * @endcode
 * @see rl::VertexArray rl::VertexBuffer rl::IndexBuffer rl::Shader
 */

/**
 * @class rl::VertexBuffer
 * @code{.cpp}
 * float vertices[] = {
 *      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
 *      0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
 *      0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
 *      -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
 *  };
 *  rl::Ref<rl::VertexBuffer> vb = rl::VertexBuffer::Create(vertices, sizeof(vertices));
 *  rl::BufferLayout layout = {
 *      {rl::ShaderDataType::Float3, "a_Position"},
 *      {rl::ShaderDataType::Float2, "a_TexCord"}
 *  };
 *  vb->setLayout(layout);
 *  m_texturedSquare->setVertexBuffer(vb);  // Reference: rl::VertexArray
 * @endcode
 * @see rl::VertexArray rl::BufferLayout rl::Shader
 */

/**
 * @class rl::IndexBuffer
 * @code{.cpp}
 * uint32_t indices[] = {0, 1, 2, 2, 3, 0};
 * rl::Ref<rl::IndexBuffer> ib = rl::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
 * m_texturedSquare->setIndexBuffer(ib);
 * @endcode
 * @see rl::VertexArray rl::VertexBuffer
 */
