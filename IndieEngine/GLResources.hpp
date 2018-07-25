/**
* @file		GLResources.hpp
* @author	Shinjihong
* @date		19 July 2018
* @version	1.0.0
* @brief	provide user easy creeating of OpenGL Resources like texture, cubemap, buffer i.e. .
* @see
*/

#ifndef GL_RESOURCES_HPP
#define GL_RESOURCES_HPP
#include <vector>

class GLResources
{
private:

public:
	static unsigned int createColorbuffer(int width, int height, bool hdr);
	static unsigned int createDepthStencilRBO(int width, int height);
	static unsigned int createCubeMap(const std::vector<std::string>& faces);
	static unsigned int createTexture(const std::string& path, bool gamma);
};

#endif