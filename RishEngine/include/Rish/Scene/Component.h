#pragma once

#include "Rish/rlpch.h"

#include "glm/glm.hpp"

// For testing
#include "Rish/Renderer/Buffer.h"
#include "Rish/Renderer/VertexArray.h"
#include "Rish/Renderer/Shader.h"
#include "Rish/Renderer/Texture2D.h"
#include "Rish/Core/VFS.h"
#include "cereal/cereal.hpp"

namespace glm
{

	template <class Archive>
	void serialize(Archive &archive, glm::vec2 &v) { archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(cereal::make_nvp("x" ,v.x), cereal::make_nvp("y" ,v.y), cereal::make_nvp("z" ,v.z)); }
	template<class Archive> void serialize(Archive& archive, glm::vec4& v) 
	{
		 archive(cereal::make_nvp("x" ,v.x), 
		 		 cereal::make_nvp("y" ,v.y), 
		 		 cereal::make_nvp("z" ,v.z), 
		 		 cereal::make_nvp("w" ,v.w)
		); 
	}
	
	
	template<class Archive> void serialize(Archive& archive, glm::ivec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::ivec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::ivec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::uvec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::uvec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::uvec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::dvec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::dvec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::dvec4& v) { archive(v.x, v.y, v.z, v.w); }

	// glm matrices serialization
	template<class Archive> void serialize(Archive& archive, glm::mat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::dmat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::mat3& m) { archive(m[0], m[1], m[2]); }
	template <class Archive> void serialize(Archive &archive, glm::mat4 &m) { archive(cereal::make_nvp("mat4 0", m[0]), cereal::make_nvp("mat4 1", m[1]), cereal::make_nvp("mat4 2", m[2]), cereal::make_nvp("mat4 3", m[3])); }
	template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(m[0], m[1], m[2], m[3]); }

	template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(q.x, q.y, q.z, q.w); }
	template<class Archive> void serialize(Archive& archive, glm::dquat& q) { archive(q.x, q.y, q.z, q.w); }

}

namespace rl {

struct TagComponent
{
	std::string tag;

	int test = 0;

	TagComponent() = default;
	TagComponent(const std::string& tag_)
		: tag(tag_){}



private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive &ar)
	{
		ar(cereal::make_nvp("Tag", tag));
	}

};

struct TransformComponent
{
	glm::mat4 transform {1.0f};
	glm::vec3 translate {0.5f, 0.1f, 0.0f};
	glm::vec3 scale {1.f, 1.f, 1.f};

	int test = 1;

	TransformComponent() = default;
	TransformComponent(const glm::mat4& transform)
		: transform(transform){}

private:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive &ar)
	{
		ar( cereal::make_nvp("Transform", transform),
			cereal::make_nvp("Translate", translate),
			cereal::make_nvp("Scale", scale)
		);
	}
};

struct RenderComponent
{

	int test = 2;

	// color
	glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

	std::string vertPath = "/shader/vertexShader/vertexSrc.glsl";
	std::string fragPath = "/shader/fragmentShader/fragSrc.glsl";
	std::shared_ptr<rl::Shader> m_shader;

	bool reload = true;

	// vao
	std::shared_ptr<VertexArray> m_vertexArray;
	std::string path = "assets/texture/1.png";
	std::shared_ptr<Texture2D> m_texture;

	bool reloadTexture = true;
	bool init = true;

	float vertices[5 * 4] = {
		//     ---- 位置 ----
	     0.1f,  0.1f, 0.0f, 1.0f, 1.0f,
		 0.1f, -0.1f, 0.0f, 1.0f, 0.0f,
		-0.1f, -0.1f, 0.0f, 0.0f, 0.0f,
		-0.1f,  0.1f, 0.0f, 0.0f, 1.0f
	};

	uint32_t indices[2*3] = {
		0, 1, 3, 
		1, 2, 3	 
	};

	RenderComponent() = default;
	RenderComponent(const std::shared_ptr<rl::Shader> shader) : m_shader(shader) {}
	RenderComponent(const std::string &vertexPath, const std::string &fragmentPath, const glm::vec4 &color)
		: vertPath(vertexPath), fragPath(fragmentPath), color(color)
	{
	}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive &ar)
	{
		ar(cereal::make_nvp("Color", color),
		   cereal::make_nvp("Vertex Shader", vertPath),
		   cereal::make_nvp("Fragment Shader", fragPath),
		   cereal::make_nvp("Texture", path)
		);
	}

};

}



