#pragma once

#include <Rish/rlpch.h>

#include <Rish/Renderer/Buffer.h>
#include <Rish/Renderer/VertexArray.h>
#include <Rish/Renderer/Shader.h>
#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/Camera/Camera.h>
#include <Rish/Core/VFS.h>

#include <cereal/cereal.hpp>

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
	template<class Archive> void serialize(Archive &archive, glm::mat4 &m) { archive(cereal::make_nvp("col0", m[0]), cereal::make_nvp("col1", m[1]), cereal::make_nvp("col2", m[2]), cereal::make_nvp("col3", m[3])); }
	template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(m[0], m[1], m[2], m[3]); }

	template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(q.x, q.y, q.z, q.w); }
	template<class Archive> void serialize(Archive& archive, glm::dquat& q) { archive(q.x, q.y, q.z, q.w); }

}

namespace rl {

/**
 * @defgroup components Components
 * @brief Components for entities
 * @ingroup scene
 * @{
 */

/**
 * @brief Tag
 */
struct TagComponent
{
	std::string tag{};
	std::string id{};

	TagComponent() = default;
	TagComponent(const std::string& t) : tag(t) {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive &ar)
	{
		ar(cereal::make_nvp("Tag", tag), cereal::make_nvp("Id", id));
	}

};

/**
 * @brief Transform
 */
struct TransformComponent
{
	glm::mat4 transform {1.0f};
	glm::vec3 translate {0.0f, 0.0f, 0.0f};
	glm::vec3 scale {1.f, 1.f, 1.f};

	TransformComponent() = default;
	TransformComponent(const glm::mat4& t) : transform(t) {}

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

/**
 * @brief Render
 */
struct RenderComponent
{
	std::string vertPath = "/shader/vertexSrc.glsl";
	std::string fragPath = "/shader/fragSrc.glsl";
	std::shared_ptr<rl::Shader> m_shader;

	glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
	std::string texturePath = "assets/texture/1.png";
	std::shared_ptr<Texture2D> m_texture;

	// states
	bool init = true;

	RenderComponent() = default;
	RenderComponent(const std::shared_ptr<rl::Shader> s) : m_shader(s) {}
	RenderComponent(const std::string &vp, const std::string &fp, const glm::vec4 &c)
		: vertPath(vp), fragPath(fp), color(c)
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
		   cereal::make_nvp("Texture", texturePath)
		);
	}

};

/**
 * @brief Camera Component
 */
struct CameraComponent
{
    Camera camera;
    float zoom = 1.f;


    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
    CameraComponent(const glm::mat4 &c)
        : camera(c) {}
};

/**
 * @}
 */

} // end of rl
