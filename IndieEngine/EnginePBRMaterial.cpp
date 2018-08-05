#include "EnginePBRMaterial.hpp"
#include <glad/glad.h>
#include "GLResources.hpp"

EnginePBRMaterial::EnginePBRMaterial(const std::string& path)
	: albedoMap(0), normalMap(0), metallicMap(0), roughnessMap(0), aoMap(0)
{	  
	loadMaterial(path);
}

EnginePBRMaterial::~EnginePBRMaterial()
{
	glDeleteTextures(1, &albedoMap);
	glDeleteTextures(1, &normalMap);
	glDeleteTextures(1, &metallicMap);
	glDeleteTextures(1, &roughnessMap);
	glDeleteTextures(1, &aoMap);
}

bool EnginePBRMaterial::loadMaterial(const std::string & path)
{
	if ((albedoMap = GLResources::createTexture(path + "albedo.png", true)) == 0)
		return false;

	if ((metallicMap = GLResources::createTexture(path + "metallic.png", true)) == 0)
		return false;

	if ((normalMap = GLResources::createTexture(path + "normal.png", true)) == 0)
		return false;

	if ((aoMap = GLResources::createTexture(path + "ao.png", true)) == 0)
		return false;

	if ((roughnessMap = GLResources::createTexture(path + "roughness.png", true)) == 0)
		return false;

	return true;
}

void EnginePBRMaterial::bindMaterial(void) const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metallicMap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, roughnessMap);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, aoMap);
}