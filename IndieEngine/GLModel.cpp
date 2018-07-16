#include "GLModel.hpp"
#include "EngineLogger.hpp"
#include <fstream>
#include <limits>
#include <chrono>

GLModel::GLModel()
	: posStack(), normStack(), uvStack(), posIdxStack(), normIdxStack(), uvIdxStack()
{
}

GLModel::GLModel(const std::string & modelPath, bool verbose, bool normalization)
{
	loadModelFromObjFile(modelPath, verbose, normalization);
}

void GLModel::loadModelFromObjFile(const std::string & modelPath, bool verbose, bool normalization)
{
	const auto start = std::chrono::system_clock::now();

	EngineLogger::getInstance()->getConsole()->info("Start reading Obj File [ {} ], Verbose : {}, Normalization : {}", 
		modelPath, verbose ? "ON" : "OFF", normalization ? "ON" : "OFF");	

	bool read_vt(false), read_vn(false);

	std::ifstream file;

	file.open(modelPath);
	if (!file.is_open()) {
		EngineLogger::getInstance()->getConsole()->error("While reading obj file [ {} ], an error occurred", modelPath);
		return;
	}

	int count = 0;

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
			unsigned int v[3], vt[3], vn[3];
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

			posIdxStack.emplace_back(glm::uvec3(v[0], v[1], v[2]));
			if (read_vt)
				uvIdxStack.emplace_back(glm::uvec3(vt[0], vt[1], vt[2]));
			if (read_vn)
				normIdxStack.emplace_back(glm::uvec3(vn[0], vn[1], vn[2]));
		}
		else file.getline(buffer, 255);

		if (count == 1500) count = 0;
	}

	const auto end = std::chrono::system_clock::now();
	const auto duration = std::chrono::duration<double>(end - start);

	file.close();
	EngineLogger::getInstance()->getConsole()->info("Reading Obj File [ {} ] is finished | {} (s)", modelPath, duration.count());
	EngineLogger::getInstance()->getConsole()->info("#Vertices : {}, #Indices : {}", posStack.size(), posIdxStack.size());


	if (normalization)
		scaleToUnitBox();
}

void GLModel::scaleToUnitBox(float cardinality)
{
	const auto start = std::chrono::system_clock::now();

	const float minFloat = std::numeric_limits<float>::min();
	const float maxFloat = std::numeric_limits<float>::max();
	
	glm::vec3 bbMin = glm::vec3(maxFloat, maxFloat, maxFloat);
	glm::vec3 bbMax = glm::vec3(minFloat, minFloat, minFloat);
			  
	for (const auto& pos : posStack)
	{
		bbMin.x = std::min(pos.x, bbMin.x);
		bbMin.y = std::min(pos.y, bbMin.y);
		bbMin.z = std::min(pos.z, bbMin.z);
		bbMax.x = std::max(pos.x, bbMax.x);
		bbMax.y = std::max(pos.y, bbMax.y);
		bbMax.z = std::max(pos.z, bbMax.z);
	}
	
	EngineLogger::getInstance()->getConsole()->info("Bounding Box min : ({}, {}, {}), max : ({}, {}, {})", 
		bbMin.x, bbMin.y, bbMin.z, bbMax.x, bbMax.y, bbMax.z);

	const float dx = bbMax.x - bbMin.x;
	const float dy = bbMax.y - bbMin.y;
	const float dz = bbMax.z - bbMin.z;

	const float dm = std::max(std::max(dx, dy), dz);
	const float inv_dm = 1.f / dm;

	for (auto& position : posStack)
	{
		position -= bbMin;
		position *= cardinality * inv_dm;
	}

	const auto end = std::chrono::system_clock::now();
	const auto duration = std::chrono::duration<double>(end - start);
	EngineLogger::getInstance()->getConsole()->info("Scaling model finished | {} (s)", duration.count());
}

void GLModel::moveToMesh(GLMesh & targetMesh) const
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const int numIndices = static_cast<int>(posIdxStack.size());
	Vertex tempVertex;

	for (int i = 0; i < numIndices; ++i)
	{
		for (int idx = 0; idx < 3; ++idx)
		{
			tempVertex = { posStack[posIdxStack[i][idx]], normStack[normIdxStack[i][idx]], uvStack[uvIdxStack[i][idx]] };
			vertices.emplace_back(std::move(tempVertex));
		}
	}

	targetMesh.setupMesh(std::move(vertices), std::move(indices));
}
