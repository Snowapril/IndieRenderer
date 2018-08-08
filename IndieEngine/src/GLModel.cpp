#include "../include/GLModel.hpp"
#include "../include/EngineLogger.hpp"
#include <fstream>
#include <limits>
#include <chrono>
#include <unordered_map>
#include <sstream>
#include "../include/EngineProfiler.hpp"


GLModel::GLModel()
{
}

GLModel::GLModel(const std::string & modelPath)
{
	loadModel(modelPath);
}

bool GLModel::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		EngineLogger::getConsole()->critical("Assimp error : {:15}", importer.GetErrorString());
		return false;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

	scaleToUnitBox(1.f);

	for (auto& mesh : meshes)
		mesh.bindBuffer();

	return true;
}

void GLModel::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void GLModel::scaleToUnitBox(float cardinality)
{
	const float minFloat = std::numeric_limits<float>::min();
	const float maxFloat = std::numeric_limits<float>::max();

	glm::vec3 bbMin = glm::vec3(maxFloat, maxFloat, maxFloat);
	glm::vec3 bbMax = glm::vec3(minFloat, minFloat, minFloat);

	for (int numMesh = 0; numMesh < meshes.size(); ++numMesh)
	{
		for (const auto& vertex : meshes[numMesh].vertices)
		{
			bbMin.x = std::min(vertex.Position.x, bbMin.x);
			bbMin.y = std::min(vertex.Position.y, bbMin.y);
			bbMin.z = std::min(vertex.Position.z, bbMin.z);
			bbMax.x = std::max(vertex.Position.x, bbMax.x);
			bbMax.y = std::max(vertex.Position.y, bbMax.y);
			bbMax.z = std::max(vertex.Position.z, bbMax.z);
		}
	}

	EngineLogger::getConsole()->info("Bounding Box min : ({}, {}, {}), max : ({}, {}, {})",
		bbMin.x, bbMin.y, bbMin.z, bbMax.x, bbMax.y, bbMax.z);

	const float dx = bbMax.x - bbMin.x;
	const float dy = bbMax.y - bbMin.y;
	const float dz = bbMax.z - bbMin.z;

	const float dm = std::max(std::max(dx, dy), dz);
	const float inv_dm = 1.f / dm;

	const glm::vec3 halfDelta = glm::vec3(dx, dy, dz) / 2.f;

	for (int numMesh = 0; numMesh < meshes.size(); ++numMesh)
	{
		for (auto& vertex : meshes[numMesh].vertices)
		{
			vertex.Position -= bbMin + halfDelta;
			vertex.Position *= cardinality * inv_dm;
		}
	}
	EngineLogger::getConsole()->info("Scaling model finished.");
}

GLMesh GLModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return GLMesh(vertices, indices);
}

void GLModel::drawModel(unsigned int drawMode) const
{
	for (const auto& mesh : meshes)
		mesh.drawMesh(drawMode);
}