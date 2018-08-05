#ifndef ENGINE_PBR_MATERIAL_HPP
#define ENGINE_PBR_MATERIAL_HPP

#include <string>

class EnginePBRMaterial
{
private:
	using GLuint = unsigned int;

	GLuint albedoMap;
	GLuint normalMap;
	GLuint metallicMap;
	GLuint roughnessMap;
	GLuint aoMap;
public:
	EnginePBRMaterial(const std::string& path);
	~EnginePBRMaterial();

	bool loadMaterial(const std::string& path);
	void bindMaterial(void) const;
};

#endif