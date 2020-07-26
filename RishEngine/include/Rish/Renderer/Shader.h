/**
 * @file Shader.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Header of Shader
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Core/Log.h>
#include <glm/glm.hpp>


namespace rl {

/**
 * @brief Shader
 */
class RL_API Shader
{
public:
    /**
     * @brief ctor
     * @param vertPath Path to vertex shader source file
     * @param fragPath Path to fragment shader source file
     */
	Shader(const char *vertPath, const char *fragPath);
	~Shader()
	{
		glDeleteProgram(program);
	}

	void bind();
	void unbind();

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
	static uint32_t CompileShader(GLenum type, const char **src);
	static uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment);

private:
	std::string m_vertSource, m_fragSource;
	uint32_t program = 0;
};

}
