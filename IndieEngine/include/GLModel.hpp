/**
* @file		GLModel.hpp
* @author	Shinjihong
* @date		17 July 2018
* @version	1.0.0
* @brief	OpenGL Model class for IndieEngine. 
* @details  
* @see
*/

#ifndef GL_MODEL_HPP
#define GL_MODEL_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "GLMesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

	
class GLModel
{
private:
	std::vector<GLMesh> meshes; 
	
	std::string directory;

	void processNode(aiNode* node, const aiScene* scene);
	GLMesh processMesh(aiMesh* mesh, const aiScene* scene);
	void scaleToUnitBox(float cardinality);
public:
	bool loadModel(const std::string& path);

	GLModel();
	GLModel(const std::string& modelPath);

	/// draw call for model. 
	void drawModel(unsigned int drawMode) const;
};

#endif