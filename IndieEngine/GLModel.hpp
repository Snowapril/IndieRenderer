/**
* @file		GLModel.hpp
* @author	Shinjihong
* @date		17 July 2018
* @version	1.0.0
* @brief	OpenGL Model class for IndieEngine. 
* @details  User can call CheckError() macro where user want to debugging. if errors occurred before CheckError() be called, then all of them will be logged into the console.
* @see
*/

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
	std::vector<GLMesh> meshes; 
	
	/// this needed for loading indices from obj file.
	static unsigned int getHash(const char* str); 
public:

	GLModel();
	GLModel(const std::string& modelPath, bool verbose = false, bool normalization = true);

	/// load obj file into meshes vector. if verbose is true, all loading information logged into the console.
	/// if normalization is true, loaded model will be scaled into 1x1x1 unit box.
	void loadModelFromObjFile(const std::string& modelPath, bool verbose = false, bool normalization = true); 
	/// scale loaded model to unit size box.
	void scaleToUnitBox(float cardinality = 1.f); 
	/// draw call for model. 
	void drawModel(void) const; 
	std::string getModelName(void) const { return modelName; }
};

#endif