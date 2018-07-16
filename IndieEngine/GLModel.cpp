#include "GLModel.hpp"
#include "EngineLogger.hpp"
#include <fstream>
#include <limits>
#include <chrono>
#include <unordered_map>
#include <sstream>


GLModel::GLModel()
{
}

GLModel::GLModel(const std::string & modelPath, bool verbose, bool normalization)
{
	loadModelFromObjFile(modelPath, verbose, normalization);
}

void GLModel::loadModelFromObjFile(const std::string & modelPath, bool verbose, bool normalization)
{
	const auto start = std::chrono::system_clock::now();

	EngineLogger::getInstance()->getConsole()->info("Start reading Obj File [ {} ]", modelPath);
	EngineLogger::getInstance()->getConsole()->info("Verbose: {}, Normalization : {}", verbose ? "ON" : "OFF", normalization ? "ON" : "OFF");

	bool read_vt(false), read_vn(false);

	std::ifstream file;

	file.open(modelPath);
	if (!file.is_open()) {
		EngineLogger::getInstance()->getConsole()->error("While reading obj file [ {} ], an error occurred", modelPath);
		return;
	}

	int count = 0;
	unsigned int idxCount = 0;
	std::unordered_map<unsigned int, unsigned int> indexMap;
	std::vector<glm::vec3> posStack;
	std::vector<glm::vec3> normStack;
	std::vector<glm::vec2> uvStack;

	char buffer[255];
	while (file >> buffer)
	{
		++count;

		if (strcmp(buffer, "#") == 0)
		{
			file.getline(buffer, 255);
			if (verbose) EngineLogger::getInstance()->getConsole()->info("Comment : {}", buffer);
		}
		else if (strcmp(buffer, "v") == 0) // vertices
		{
			float x, y, z;
			file >> x >> y >> z;

			posStack.emplace_back(glm::vec3(x, y, z));
			if (verbose && count == 1500) EngineLogger::getInstance()->getConsole()->info("Vertex : ({}, {}, {})", x, y, z);
		}
		else if (strcmp(buffer, "vt") == 0) //uv tex coordinates
		{
			read_vt = true;

			float u, v;
			file >> u >> v;

			uvStack.emplace_back(glm::vec2(u, v));
			if (verbose && count == 1500) EngineLogger::getInstance()->getConsole()->info("UV : ({}, {})", u, v);
		}
		else if (strcmp(buffer, "vn") == 0) //vertex normals
		{
			read_vn = true;

			float x, y, z;
			file >> x >> y >> z;

			normStack.emplace_back(glm::vec3(x, y, z));
			if (verbose && count == 1500) EngineLogger::getInstance()->getConsole()->info("Normal : ({}, {}, {})", x, y, z);
		}
		else if (strcmp(buffer, "f") == 0) //faces
		{
			unsigned int v[3], vt[3] = { 0, }, vn[3] = { 0, };
			if (read_vt && read_vt)
			{
				for (int i = 0; i < 3; ++i)
				{
					file >> v[i]; file.get(buffer, 2);
					file >> vt[i]; file.get(buffer, 2);
					file >> vn[i];

					--v[i];
					--vt[i];
					--vn[i];
				}
			}
			else if (read_vt && !read_vn)
			{
				for (int i = 0; i < 3; ++i)
				{
					file >> v[i]; file.get(buffer, 2); file.get(buffer, 2);
					file >> vt[i];

					--v[i];
					--vt[i];
				}
			}
			else if (!read_vt && read_vn)
			{
				for (int i = 0; i < 3; ++i)
				{
					file >> v[i]; file.get(buffer, 2); file.get(buffer, 2);
					file >> vn[i];

					--v[i];
					--vn[i];
				}
			}
			else
			{
				for (int i = 0; i < 3; ++i)
				{
					file >> v[i];

					--v[i];
				}
			}

			if (verbose && count == 1500)
				EngineLogger::getInstance()->getConsole()->info("Vertex face : ({}, {}, {})", v[0], v[1], v[2]);

			unsigned int synthesizedIndex;
			Vertex vertex;
			std::string hashStr;
			unsigned int hashKey;

			for (int i = 0; i < 3; ++i) {
				hashStr = std::to_string(v[i]) + std::to_string(vn[i]) + std::to_string(vt[i]);
				hashKey = getHash(hashStr.c_str());
				if (indexMap.find(hashKey) != indexMap.end())
				{
					synthesizedIndex = indexMap[hashKey];
				}
				else
				{
					synthesizedIndex = idxCount++;
					indexMap[hashKey] = synthesizedIndex;

					vertex = { posStack[v[i]], normStack[vn[i]], uvStack[vt[i]] };
					meshes.back().vertices.push_back(vertex);
				}
				meshes.back().indices.push_back(synthesizedIndex);
				hashStr.clear();
			}
		}
		else if (strcmp(buffer, "o") == 0)
		{
			meshes.push_back(GLMesh());
			std::string meshName;

			file >> meshName;
			meshes.back().meshName = meshName;

			indexMap.clear();
			idxCount = 0;
		}
		else file.getline(buffer, 255);

		if (count == 1500) count = 0;
	}

	const auto end = std::chrono::system_clock::now();
	const auto duration = std::chrono::duration<double>(end - start);

	file.close();
	EngineLogger::getInstance()->getConsole()->info("Reading Obj File [ {} ] is finished | {} (s)", modelPath, duration.count());

	if (normalization)
		scaleToUnitBox();

	for (auto& mesh : meshes)
		mesh.bindBuffer();
}

void GLModel::scaleToUnitBox(float cardinality)
{
	const auto start = std::chrono::system_clock::now();

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

	EngineLogger::getInstance()->getConsole()->info("Bounding Box min : ({}, {}, {}), max : ({}, {}, {})",
		bbMin.x, bbMin.y, bbMin.z, bbMax.x, bbMax.y, bbMax.z);

	const float dx = bbMax.x - bbMin.x;
	const float dy = bbMax.y - bbMin.y;
	const float dz = bbMax.z - bbMin.z;

	const float dm = std::max(std::max(dx, dy), dz);
	const float inv_dm = 1.f / dm;

	for (int numMesh = 0; numMesh < meshes.size(); ++numMesh)
	{
		for (auto& vertex : meshes[numMesh].vertices)
		{
			vertex.Position -= bbMin;
			vertex.Position *= cardinality * inv_dm;
		}
	}

	const auto end = std::chrono::system_clock::now();
	const auto duration = std::chrono::duration<double>(end - start);
	EngineLogger::getInstance()->getConsole()->info("Scaling model finished | {} (s)", duration.count());
}

void GLModel::drawModel(void) const
{
	for (const auto& mesh : meshes)
		mesh.drawMesh();
}

//from https://stackoverflow.com/questions/8317508/hash-function-for-a-string
unsigned int GLModel::getHash(const char* str)
{
	unsigned h = 37;
	while (*str) {
		h = (h * 54059) ^ (str[0] * 76963);
		str++;
	}
	return h % 86969;
}