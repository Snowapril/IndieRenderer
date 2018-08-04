/**
* @file		GLMesh.hpp
* @author	Shinjihong
* @date		17 July 2018
* @version	1.0.0
* @brief	OpenGL Mesh in IndieEngine. 
* @details	Mesh contains vertices and indices. vertex is composed with position, normal, uv vectors.
* @see		
*/

#ifndef GL_MESH_HPP
#define GL_MESH_HPP

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

typedef struct _Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
} Vertex;

enum class IndieShape {
	INDIE_BOX		= 0,
	INDIE_SPHERE	= 1,
};

class GLMesh
{
	friend class GLModel;
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int VAO;
	std::string meshName;
private:
	void bindBuffer(void);

public:
	GLMesh();
	GLMesh(GLMesh&& other);
	GLMesh& operator=(GLMesh&& other);
	GLMesh(IndieShape shape);
	GLMesh(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices);
	GLMesh(std::vector<Vertex>&& _vertices, std::vector<unsigned int>&& _indices);
	~GLMesh();

	unsigned int getVertexArrayObject(void) const { return VAO; }

	void setupMesh(void);
	void setupMesh(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices);
	void setupMesh(std::vector<Vertex>&& _vertices, std::vector<unsigned int>&& _indices);

	void setupWithFixedGeometryShape(IndieShape shape);

	void drawMesh(unsigned int drawMode) const;
	std::string getMeshName(void) const { return meshName; }
};


#endif