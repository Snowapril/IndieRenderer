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


class GLMesh
{
private:
	std::vector<Vertex> vertices;
	std::vector<glm::uvec3> indices;

	unsigned int VAO;
public:
	GLMesh();
	GLMesh(const std::vector<Vertex>& _vertices, const std::vector<glm::uvec3>& _indices);
	GLMesh(std::vector<Vertex>&& _vertices, std::vector<glm::uvec3>&& _indices);
	~GLMesh();

	unsigned int getVertexArrayObject(void) const { return VAO; }

	void bindBuffer(void);
	void bindBuffer(const std::vector<Vertex>& _vertices, const std::vector<glm::uvec3>& _indices);
	void bindBuffer(std::vector<Vertex>&& _vertices, std::vector<glm::uvec3>&& _indices);

	void drawMesh(void) const;
};


#endif