/**
 * @file Shader.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Header of Shader
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <Rish/rlpch.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace rl {

// TODO: Support load shaders in one file

/**
 * @brief Shader
 */
class RL_API Shader
{
public:
    /**
     * @brief ctor
     * @param vertSrc Path to vertex shader source file
     * @param fragSrc Path to fragment shader source file
     */
	Shader(const char *vertSrc, const char *fragSrc);
	/**
	 * @brief dtor
	 */
	~Shader();

	static std::string s_vertSource, s_fragSource;

	/**
	 * @brief Load a Shader using rl::FileSystem
	 * @param vertPath Path to vertex shader
	 * @param fragPath Path to fragment shader
	 * @return Reference to the Shader
	 */
	static Ref<Shader> LoadShader(const std::string &vertPath, const std::string &fragPath);

	/**
	 * @brief Load a Shader using rl::VFS
	 * @param vertPath Path to vertex shader
	 * @param fragPath Path to fragment shader
	 * @return Reference to the Shader
	 */
	static Ref<Shader> LoadShaderVFS(const std::string &v_vertPath, const std::string &v_fragPath);

	void bind();   ///< Bind
	void unbind(); ///< Unbind

	uint32_t getShaderID() const { return program; }
	uint32_t getLocationByName(const std::string &name) const;

	void setInt(const std::string &name, int value);
	void setIntArray(const std::string &name, int *values, uint32_t count);
	//
	void setFloat(const std::string &name, float value);
	void setFloat2(const std::string &name, const glm::vec2 &value);
	void setFloat3(const std::string &name, const glm::vec3 &value);
	void setFloat4(const std::string &name, const glm::vec4 &value);
	//
	void setMat2(const std::string &name, const glm::mat2 &matrix);
	void setMat3(const std::string &name, const glm::mat3 &matrix);
	void setMat4(const std::string &name, const glm::mat4 &matrix);
	//
private:
	static uint32_t CompileShader(GLenum type, const char **src);
	static uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment);

	std::string m_vertSource, m_fragSource;
	uint32_t program = 0;
};

/**
 * @brief Shader Library
 */
class RL_API ShaderLibrary
{
public:
    ShaderLibrary();

    /**
     * @brief Load a Shader and add to Library
     * @param name Name of the Shader
     * @param vpath Path for both vertex shader and fragment shader (without ext)
     * @return Reference to the Shader
     */
    Ref<Shader> load(const std::string &name, const std::string &vpath);

    /**
     * @brief Get a Shader by name
     * @param name Name
     * @return Reference to the Shader
     */
    Ref<Shader> get(const std::string &name);

private:
    std::unordered_map<std::string, Ref<Shader>> m_shader; ///< Shader mapping
    Ref<Shader> m_defaultShader; ///< Default Shader
};

}

/**
 * @class rl::ShaderLibrary
 *
 * @code{.cpp}
 * rl::ShaderLibrary m_shaders;
 * rl::Ref<rl::Shader> shader = m_shaders.load("textured", "/shader/textured");
 * // Load "/shader/textured.vert" and "/shader/textured.frag" in VFS
 * @endcode
 */

/**
 * @class rl::Shader
 *
 * @code{.cpp}
 * // Load a shader
 * rl::Ref<rl::Shader> shader = LoadShader("/shader/example.vert", "/shader/example.frag");
 * // Bind
 * shader->bind();
 * // Upload uniforms
 * shader->setInt("u_Texture", 0);
 * shader->setMat4("u_ViewProjection", glm::mat4(1.f));
 * // Unbind
 * shader->unbind();
 * @endcode
 */
