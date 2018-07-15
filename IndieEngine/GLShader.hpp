#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>


class GLShader
{
private:
	unsigned int programID;
public:
	GLShader();
	GLShader(const char* vs_path, const char* fs_path, const char* gs_path = nullptr);
	~GLShader();

	void setupShader(const char* vs_path, const char* fs_path, const char* gs_path = nullptr);

	void useProgram(void) const;
	int getUniformLocation(const std::string& varName) const;

	void sendUniform(const std::string& varName, int i) const;
	void sendUniform(const std::string& varName, bool b) const;
	void sendUniform(const std::string& varName, const glm::vec3& vec) const;
	void sendUniform(const std::string& varName, const glm::vec4& vec) const;
	void sendUniform(const std::string& varName, const glm::mat4& mat) const;
};


#endif