#pragma once

#include "Rish/rlpch.h"

#include "glm/glm.hpp"

// For testing
#include "Rish/Renderer/Buffer.h"
#include "Rish/Renderer/VertexArray.h"
#include "Rish/Renderer/Shader.h"
#include "Rish/Renderer/Texture2D.h"
#include "Rish/Core/VFS.h"

namespace rl {

struct TagComponent
{
	std::string tag;

	TagComponent() = default;
	TagComponent(const std::string& tag)
		: tag(tag){}
};

struct TransformComponent
{
	glm::mat4 transform {1.0f};
	glm::vec3 translate {0.5f, 0.1f, 0.0f};
	glm::vec3 scale {1.f, 1.f, 1.f};

	TransformComponent() = default;
	TransformComponent(const glm::mat4& transform)
		: transform(transform){}

};

struct SpriteRendererComponent
{
	glm::vec4 color {1.0f, 1.0f, 1.0f, 1.0f};

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const glm::vec4& color)
		: color(color){}
};

struct ShaderComponent
{
	std::string vertPath = "/shader/vertexShader/vertexSrc.glsl";
	std::string fragPath = "/shader/fragmentShader/fragSrc.glsl";
	std::shared_ptr<rl::Shader> m_shader;

	bool reload = false;

	ShaderComponent()
	{
		VFS::Get()->ResolvePhysicalPath(vertPath, vertPath);
		VFS::Get()->ResolvePhysicalPath(fragPath, fragPath);

		m_shader = std::make_shared<rl::Shader>(vertPath.c_str(), fragPath.c_str());
	}
	ShaderComponent(const std::shared_ptr<rl::Shader> shader): m_shader(shader){}
	ShaderComponent(const std::string &vertexPath, const std::string &fragmentPath)
		: vertPath(vertexPath), fragPath(fragmentPath)
	{
		VFS::Get()->ResolvePhysicalPath(vertexPath, vertPath);
		VFS::Get()->ResolvePhysicalPath(fragmentPath, fragPath);

		m_shader = std::make_shared<rl::Shader>(vertPath.c_str(), fragPath.c_str());
	}
};

// For testing Component
struct TestQuadComponent
{
	std::shared_ptr<VertexArray> m_vertexArray;
	std::string path = "Editor/assets/texture/1.png";
	std::shared_ptr<Texture2D> m_texture;

	bool reload = true;

	float vertices[5 * 4] = {
		//     ---- 位置 ----
	     0.1f,  0.1f, 0.0f, 1.0f, 1.0f,
		 0.1f, -0.1f, 0.0f, 1.0f, 0.0f,
		-0.1f, -0.1f, 0.0f, 0.0f, 0.0f,
		-0.1f,  0.1f, 0.0f, 0.0f, 1.0f
	};

	TestQuadComponent() = default;
	TestQuadComponent(const std::shared_ptr<VertexArray> vertexArray)
	{
		RL_CORE_TRACE("[TestQuadComponent] Construct test quad component" );
		m_vertexArray = vertexArray;

		uint32_t indices[2*3] = {
			0, 1, 3, 
			1, 2, 3	 
		};
		std::shared_ptr<rl::VertexBuffer> vertexBuffer;
		vertexBuffer = std::make_shared<rl::VertexBuffer>(vertices, sizeof(vertices));
		rl::BufferLayout layout = {
			{rl::ShaderDataType::Float3, "a_Position"},
			{rl::ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->setLayout(layout);
		m_vertexArray->setVertexBuffer(vertexBuffer);
		std::shared_ptr<rl::IndexBuffer> indexBuffer;
		indexBuffer = std::make_shared<rl::IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
		m_vertexArray->setIndexBuffer(indexBuffer);

		m_vertexArray->unbind(); // Always remember to UNBIND if AMD
	}
};

}



