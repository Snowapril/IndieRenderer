#ifndef GL_DEBUGGER_HPP
#define GL_DEBUGGER_HPP

#include <glad/glad.h>
#include <string>
#include "EngineLogger.hpp"

class GLDebugger
{
public:
	static GLenum CheckError(const char* file, int line)
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			EngineLogger::getInstance()->getConsole()->debug("Error [ {} ] || {} ( {} )", error.c_str(), file, line);
		}

		if (errorCode == GL_NO_ERROR) EngineLogger::getInstance()->getConsole()->debug("NO ERROR at {} ( {} )\n", file, line);
		return errorCode;
	}
};

#define CheckError() GLDebugger::CheckError(__FILE__, __LINE__)

#endif