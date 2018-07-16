#ifndef GL_MODEL_HPP
#define GL_MODEL_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "GLMesh.hpp"

class GLModel
{
private:
	std::vector<glm::vec3> posStack;
	std::vector<glm::vec3> normStack;
	std::vector<glm::vec2> uvStack;
	std::vector<glm::uvec3> posIdxStack;
	std::vector<glm::uvec3> normIdxStack;
	std::vector<glm::uvec3> uvIdxStack;

public:
	GLModel();
	GLModel(const std::string& modelPath, bool verbose = false, bool normalization = true);

	void loadModelFromObjFile(const std::string& modelPath, bool verbose = false, bool normalization = true);
	void scaleToUnitBox(float cardinality = 1.f); //scale loaded model to unit size box.

	void moveToMesh(GLMesh& targetMesh) const; //move from GLModel class's geometry data my GLMesh class's data.
};


#endif