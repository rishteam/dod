#include "Rish/rlpch.h"
#include "Rish/Core/Log.h"
#include "glad/glad.h"
#include <glm/glm.hpp>

namespace rl {

class Shader
{
public:
	Shader(const char *vertPath, const char *fragPath);
	~Shader();

	void bind();
	void unbind();

	uint32_t getShaderID() const {return program; }

	int getLocationByName(const std::string &name);
	
	void setInt();
	void setIntArray();
	//
	void setFloat(const std::string &name, float value);
	void setFloat2(const std::string &name, const glm::vec2 &value);
	void setFloat3(const std::string &name, const glm::vec3 &value);
	void setFloat4(const std::string &name, const glm::vec4 &value);
	//
	void setMat2(const std::string &name, const glm::mat2 &matrix);
	void setMat2(const std::string &name, const glm::mat3 &matrix);
	void setMat2(const std::string &name, const glm::mat4 &matrix);
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
