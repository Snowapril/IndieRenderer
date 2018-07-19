#ifndef GL_MODEL_HPP
#define GL_MODEL_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "GLMesh.hpp"

class GLModel
{
private:
	std::string modelName;
	
	static unsigned int getHash(const char* str);
public:
	std::vector<GLMesh> meshes; //TODO : move this back to private scope

	GLModel();
	GLModel(const std::string& modelPath, bool verbose = false, bool normalization = true);

	void loadModelFromObjFile(const std::string& modelPath, bool verbose = false, bool normalization = true);
	void scaleToUnitBox(float cardinality = 1.f); //scale loaded model to unit size box.

	void drawModel(void) const;
	std::string getModelName(void) const { return modelName; }
};

#endif