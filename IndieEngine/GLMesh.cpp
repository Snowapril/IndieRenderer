#include "GLMesh.hpp"
#include <glad/glad.h>

GLMesh::GLMesh()
{
}

GLMesh::GLMesh(IndieShape shape)
{
	setupWithFixedGeometryShape(shape);
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

GLMesh::GLMesh(GLMesh&& other)
{
	if (&other != this)
	{
		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
	}
}

GLMesh& GLMesh::operator=(GLMesh&& other)
{
	if (&other != this)
	{
		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
	}

	return *this;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

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

void GLMesh::drawMesh(unsigned int drawMode) const
{
	glBindVertexArray(VAO);
	glDrawElements(drawMode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void GLMesh::setupWithFixedGeometryShape(IndieShape shape)
{
	switch (shape)
	{
	case IndieShape::INDIE_BOX : {
			std::vector<Vertex> _vertices = {
				// positions          // normals           // texture coords
				{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  0.0f) } ,
			    { glm::vec3(0.5f,  0.5f, -0.5f) , glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(0.5f, -0.5f, -0.5f) , glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(0.5f,  0.5f, -0.5f) , glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  0.0f) } ,
			    { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f,  1.0f) } ,
			    
			    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  0.0f) } ,
			    { glm::vec3(0.5f, -0.5f,  0.5f) , glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(0.5f,  0.5f,  0.5f) , glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(0.5f,  0.5f,  0.5f) , glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f,  0.0f) } ,
			    
			    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f) } ,
			    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    
			    { glm::vec3(0.5f,  0.5f,  0.5f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(0.5f, -0.5f, -0.5f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(0.5f,  0.5f, -0.5f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(0.5f, -0.5f, -0.5f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(0.5f,  0.5f,  0.5f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(0.5f, -0.5f,  0.5f) , glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f,  0.0f) } ,
			    
			    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(0.5f, -0.5f, -0.5f) , glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(0.5f, -0.5f,  0.5f) , glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(0.5f, -0.5f,  0.5f) , glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  0.0f) } ,
			    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    
			    { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(0.5f,  0.5f,  0.5f) , glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(0.5f,  0.5f, -0.5f) , glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  1.0f) } ,
			    { glm::vec3(0.5f,  0.5f,  0.5f) , glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f,  0.0f) } ,
			    { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  1.0f) } ,
			    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f,  0.0f) }
			};

			std::vector<unsigned int> _indices = {
				0, 1, 2,
				3, 4, 5,
				6, 7, 8,
				9, 10, 11,
				12, 13, 14,
				15, 16, 17,
				18, 19, 20,
				21, 22, 23,
				24, 25, 26,
				27, 28, 29,
				30, 31, 32,
				33, 34, 35
			};

			setupMesh(std::move(_vertices), std::move(_indices));
			break;
		}
	case IndieShape::INDIE_SPHERE: {
			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> _indices;

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			const float PI = 3.14159265359f;
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = true;
			for (int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (int x = 0; x <= X_SEGMENTS; ++x)
					{
						_indices.push_back(y       * (X_SEGMENTS + 1) + x);
						_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						_indices.push_back(y       * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}

			std::vector<Vertex> _vertices;
			for (int i = 0; i < positions.size(); ++i)
				_vertices.push_back({ positions[i], normals[i], uv[i]});

			setupMesh(std::move(_vertices), std::move(_indices));
			break;
		}
	}
}