#include <Rish/Core/Core.h>
#include <Rish/Renderer/Shader.h>
#include <Rish/Core/FileSystem.h>

namespace rl {

Shader::Shader(const std::string &vertSrc, const std::string &fragSrc)
{
	const char *verSrcc = m_vertSource.c_str();
	uint32_t vert = CompileShader(GL_VERTEX_SHADER, &verSrcc);
	const char *fragSrcc = m_fragSource.c_str();
	uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, &fragSrcc);
	RL_CORE_ASSERT(vert && frag, "Compiler Error on Shader");

	program = LinkShaderProgram(vert, frag);
	RL_CORE_ASSERT(program, "");

	glDeleteShader(vert);
	glDeleteShader(frag);
}

Shader::Shader(const char *vertPath, const char *fragPath)
{
	m_vertSource = FileSystem::ReadTextFile(vertPath);
	RL_CORE_ASSERT(m_vertSource != "", "[Shader] Failed to open File");
	m_fragSource = FileSystem::ReadTextFile(fragPath);
	RL_CORE_ASSERT(m_fragSource != "", "[Shader] Failed to open File");

	const char *verSrc = m_vertSource.c_str();
	uint32_t vert = CompileShader(GL_VERTEX_SHADER, &verSrc);
	const char *fragSrc = m_fragSource.c_str();
	uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, &fragSrc);
	RL_CORE_ASSERT(vert && frag, "Compiler Error on Shader");

	program = LinkShaderProgram(vert, frag);
	RL_CORE_ASSERT(program, "Failed to link shader");

	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::bind()
{
	glUseProgram(program);
}

void Shader::unbind()
{
	glUseProgram(0);
}

uint32_t Shader::CompileShader(GLenum type, const char **src)
{
	uint32_t shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, src, nullptr);
	glCompileShader(shader);

	int success;
	char log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shader, 512, nullptr, log);
		RL_CORE_ERROR("Error Shader {} compile error\n {}", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", log);
		return 0;
	}
	return shader;
}

uint32_t Shader::LinkShaderProgram(uint32_t vertex, uint32_t fragment)
{
	uint32_t program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	//
	int success;
	char log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(program, 512, nullptr, log);
		RL_CORE_ERROR("Error Shader Linking error\n %s", log);
		return 0;
	}
	return program;
}

uint32_t Shader::getLocationByName(const std::string &name) const
{
	RL_CORE_ASSERT(program > 0, "The shader is invalid");
	int location = glGetUniformLocation(program, name.c_str());
	return location;
}

void Shader::setInt(const std::string &name, int value)
{
	glUniform1i(getLocationByName(name), value);
}

void Shader::setIntArray(const std::string &name, int *values, uint32_t count)
{
    RL_CORE_ASSERT(values != nullptr, "int array is nullptr");
	glUniform1iv(getLocationByName(name), count, values);
}

void Shader::setFloat(const std::string &name, float value)
{
	glUniform1f(getLocationByName(name), value);
}

void Shader::setFloat2(const std::string &name, const glm::vec2 &value)
{
	glUniform2f(getLocationByName(name), value.x, value.y);
}

void Shader::setFloat3(const std::string &name, const glm::vec3 &value)
{
	glUniform3f(getLocationByName(name), value.x, value.y, value.z);
}

void Shader::setFloat4(const std::string &name, const glm::vec4 &value)
{
	glUniform4f(getLocationByName(name), value.x, value.y, value.z, value.w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &matrix)
{
	glUniformMatrix2fv(getLocationByName(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setMat3(const std::string &name, const glm::mat3 &matrix)
{
	glUniformMatrix3fv(getLocationByName(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &matrix)
{
	glUniformMatrix4fv(getLocationByName(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

}