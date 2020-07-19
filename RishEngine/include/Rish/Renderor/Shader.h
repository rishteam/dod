#pragma once

#include <glad/glad.h>
#include "Rish/rlpch.h"
#include "Rish/Core/Log.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace rl {

bool LoadFileContent(std::string &s, const char *path);

class Shader
{
public:
	Shader(const char *vertPath, const char *fragPath);
	~Shader()
	{
		glDeleteProgram(program);
	}

	void bind();
	void unbind();

	uint32_t getShaderID() const {return program; }

	int getLocationByName(const std::string &name);

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
	void uploadUniformFloat(const std::string &name, float value);
	void uploadUniformFloat2(const std::string &name, glm::vec2 &value);
	void uploadUniformFloat3(const std::string &name, glm::vec3 &value);
	void uploadUniformFloat4(const std::string &name, glm::vec4 &value);

	static uint32_t CompileShader(GLenum type, const char **src);
	static uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment);

private:
	std::string m_vertSource, m_fragSource;
	uint32_t program = 0;
};

}
