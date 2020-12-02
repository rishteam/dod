#include <glad/glad.h>
//
#include <Rish/Core/FileSystem.h>
#include <Rish/Core/VFS.h>
//
#include <Rish/Renderer/Shader.h>

namespace rl {

// Static variables
std::string Shader::s_vertSource;
std::string Shader::s_fragSource;

Shader::Shader(ShaderFile vertFile, ShaderFile fragFile)
{
    RL_PROFILE_FUNCTION();

    uint32_t vert = CompileShader(GL_VERTEX_SHADER, &vertFile.source);
	RL_CORE_ASSERT(vert, "Compiler Error on Vertex Shader: {}", vertFile.path);

	uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, &fragFile.source);
	RL_CORE_ASSERT(frag, "Compiler Error on Fragment Shader: {}", fragFile.path);

	program = LinkShaderProgram(vert, frag);
	RL_CORE_ASSERT(program, "Failed to link shader");

	glDeleteShader(vert);
	glDeleteShader(frag);
}

Shader::~Shader()
{
    RL_PROFILE_FUNCTION();

    glDeleteProgram(program);
}

void Shader::bind()
{
    RL_PROFILE_FUNCTION();

	glUseProgram(program);
}

void Shader::unbind()
{
    RL_PROFILE_FUNCTION();

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
		RL_CORE_ERROR("Error Shader Linking error\n {}", log);
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

void Shader::setBool(const std::string &name, int value)
{
    glUniform1i(getLocationByName(name), value);
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

Ref<Shader> Shader::LoadShader(const std::string &vertPath, const std::string &fragPath)
{
    RL_PROFILE_FUNCTION();

    s_vertSource = FileSystem::ReadTextFile(vertPath);
	RL_CORE_ASSERT(s_vertSource != "", "[Shader] Failed to open File");
	s_fragSource = FileSystem::ReadTextFile(fragPath);
	RL_CORE_ASSERT(s_fragSource != "", "[Shader] Failed to open File");
	return std::make_shared<Shader>(ShaderFile{vertPath, s_vertSource.c_str()},
                                    ShaderFile{fragPath, s_fragSource.c_str()});
}

Ref<Shader> Shader::LoadShaderVFS(const std::string &v_vertPath, const std::string &v_fragPath)
{
    RL_PROFILE_FUNCTION();

    s_vertSource = VFS::ReadTextFile(v_vertPath);
	RL_CORE_ASSERT(s_vertSource != "", "[Shader] Failed to open File");
	s_fragSource = VFS::ReadTextFile(v_fragPath);
	RL_CORE_ASSERT(s_fragSource != "", "[Shader] Failed to open File");

	std::string vertPath, fragPath;
	VFS::ResolvePhysicalPath(v_vertPath, vertPath);
    VFS::ResolvePhysicalPath(v_fragPath, fragPath);

    return std::make_shared<Shader>(ShaderFile{vertPath, s_vertSource.c_str()},
                                    ShaderFile{fragPath, s_fragSource.c_str()});
}

////////////////////////////////////////////////////////////////////////////
// Shader Library

// TODO: make default shader file?
ShaderLibrary::ShaderLibrary()
{
	const char *vert = R"glsl(
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 v_Position;
out vec4 v_Color;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
    // Pass to Frag
    v_Position = a_Position;
    v_Color = a_Color;
}
	)glsl";
	const char *frag = R"glsl(
#version 330 core

in vec3 v_Position;
in vec4 v_Color;

out vec4 color;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
}
	)glsl";
	m_defaultShader = MakeRef<Shader>(Shader::ShaderFile{"ShaderLibrary_DefaultShader", vert},
                                      Shader::ShaderFile{"ShaderLibrary_DefaultShader", frag});
}

Ref<Shader> ShaderLibrary::load(const std::string &name, const std::string &vpath)
{
    RL_PROFILE_FUNCTION();

    if(m_shader.count(name))
		m_shader.erase(name);
	// TODO: roll back to default shader when error?
	auto newShader = Shader::LoadShaderVFS(vpath+".vert", vpath+".frag");
	m_shader[name] = newShader;
	RL_CORE_TRACE("[ShaderLibrary] Loaded {} from {}.vert/.frag", name, vpath);
	return newShader;
}

Ref<Shader> ShaderLibrary::get(const std::string &name)
{
//    RL_CORE_ASSERT(m_shader.count(name), "Shader not found");
    return m_shader.count(name) ? m_shader[name] : m_defaultShader;
}


}