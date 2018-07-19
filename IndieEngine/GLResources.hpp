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