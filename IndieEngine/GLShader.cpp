#include "GLShader.hpp"
#include <glad/glad.h>
#include <iostream>
#include "EngineLogger.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

GLShader::GLShader()
	: programID(0)
{
}

GLShader::GLShader(const char* vs_path, const char* fs_path, const char* gs_path)
{
	setupShader(vs_path, fs_path, gs_path);
}

GLShader::~GLShader()
{
	glDeleteProgram(programID);
}

void GLShader::setupShader(const char* vs_path, const char* fs_path, const char* gs_path)
{
	std::string vs_string, fs_string, gs_string;

	{
		std::ifstream vsFile, fsFile, gsFile;
		vsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		fsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		gsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

		try {
			if (vs_path != nullptr) {
				vsFile.open(vs_path);
				std::ostringstream osstr;
				osstr << vsFile.rdbuf();
				vs_string = osstr.str();
				vsFile.close();
			}

			if (fs_path != nullptr) {
				fsFile.open(fs_path);
				std::ostringstream osstr;
				osstr << fsFile.rdbuf();
				fs_string = osstr.str();
				fsFile.close();
			}

			if (gs_path != nullptr) {
				gsFile.open(gs_path);
				std::ostringstream osstr;
				osstr << gsFile.rdbuf();
				gs_string = osstr.str();
				gsFile.close();
			}
		}
		catch (std::ifstream::failure e) {
			EngineLogger::getInstance()->getConsole()->critical("ifstream Failure : {:<15}", e.what());
		}
	}

	GLuint vs, fs, gs;

	const char* vs_source = vs_string.c_str(), *fs_source = fs_string.c_str(), *gs_source;
	char infoLog[512];
	int success;

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, nullptr);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, sizeof(infoLog), nullptr, infoLog);
		EngineLogger::getInstance()->getConsole()->critical("Vertex Shader Compile Failed, info log :\n{}", infoLog);
		return;
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, nullptr);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, sizeof(infoLog), nullptr, infoLog);
		EngineLogger::getInstance()->getConsole()->critical("Fragment Shader Compile Failed, info log :\n{}", infoLog);
		glDeleteShader(vs);
		return;
	}

	if (gs_path != nullptr) {
		gs_source = gs_string.c_str();
		gs = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gs, 1, &gs_source, nullptr);
		glCompileShader(gs);

		glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(gs, sizeof(infoLog), nullptr, infoLog);
			EngineLogger::getInstance()->getConsole()->critical("Geometry Shader Compile Failed, info log :\n{}", infoLog);
			glDeleteShader(vs);
			glDeleteShader(fs);
			return;
		}
	}

	programID = glCreateProgram();
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	if (gs_path != nullptr)
		glAttachShader(programID, gs);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog);
		EngineLogger::getInstance()->getConsole()->critical("Program Linking Failed, info Log : \n{}", infoLog);
		glDeleteShader(vs);
		glDeleteShader(fs);
		if (gs_path != nullptr)
			glDeleteShader(gs);
		glDeleteProgram(programID);
		return;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
	if (gs_path != nullptr)
		glDeleteShader(gs);

	EngineLogger::getInstance()->getConsole()->info("Compile Shader Sources Complete.\n\tVertex Shader   : [{}]\n\tFragment Shader : "
													"[{}]\n\tGeometry Shader : [{}]", vs_path, fs_path, gs_path == nullptr ? "None" : gs_path);
}

void GLShader::useProgram(void) const
{
	glUseProgram(programID);
}

int GLShader::getUniformLocation(const std::string & varName) const
{
	int loc = glGetUniformLocation(programID, (const GLchar *)varName.c_str());

	return loc;
}

void GLShader::sendUniform(const std::string & varName, int i) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		EngineLogger::getInstance()->getConsole()->critical("Undefined Uniform Variable Name : ");
	}
	else {
		glUniform1i(loc, i);
	}
}

void GLShader::sendUniform(const std::string & varName, bool b) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		EngineLogger::getInstance()->getConsole()->critical("Undefined Uniform Variable Name : ");
	}
	else {
		glUniform1i(loc, b);
	}
}

void GLShader::sendUniform(const std::string & varName, const glm::vec3 & vec) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		EngineLogger::getInstance()->getConsole()->critical("Undefined Uniform Variable Name : ");
	}
	else {
		glUniform3fv(loc, 1, &vec[0]);
	}
}

void GLShader::sendUniform(const std::string & varName, const glm::vec4 & vec) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		EngineLogger::getInstance()->getConsole()->critical("Undefined Uniform Variable Name : ");
	}
	else {
		glUniform4fv(loc, 1, &vec[0]);
	}
}

void GLShader::sendUniform(const std::string & varName, const glm::mat4 & mat) const
{
	int loc = getUniformLocation(varName);

	if (loc == -1) {
		EngineLogger::getInstance()->getConsole()->critical("Undefined Uniform Variable Name : ");
	}
	else {
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}
