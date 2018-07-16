#include "GLMesh.hpp"
#include <glad/glad.h>

GLMesh::GLMesh()
{
}

GLMesh::GLMesh(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices)
{
	vertices = _vertices;
	indices = _indices;

	vertices.shrink_to_fit();
	indices.shrink_to_fit();
}

GLMesh::GLMesh(std::vector<Vertex>&& _vertices, std::vector<unsigned int>&& _indices)
{
	vertices = std::move(_vertices);
	indices = std::move(_indices);

	vertices.shrink_to_fit();
	indices.shrink_to_fit();
}

GLMesh::~GLMesh()
{
	glDeleteVertexArrays(1, &VAO);
}

void GLMesh::bindBuffer(void)
{
	GLuint VBO, IBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

void GLMesh::setupMesh(void)
{
	assert(vertices.size() > 0 || indices.size() > 0);

	bindBuffer();
}

void GLMesh::setupMesh(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices)
{
	vertices = _vertices;
	indices  = _indices;

	vertices.shrink_to_fit();
	indices.shrink_to_fit();

	bindBuffer();
}

void GLMesh::setupMesh(std::vector<Vertex>&& _vertices, std::vector<unsigned int>&& _indices)
{
	vertices = std::move(_vertices);
	indices  = std::move(_indices);

	vertices.shrink_to_fit();
	indices.shrink_to_fit();

	bindBuffer();
}

void GLMesh::drawMesh(void) const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
