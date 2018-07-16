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
	INDIE_BOX = 0,
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

	void drawMesh(void) const;
	std::string getMeshName(void) const { return meshName; }
};


#endif