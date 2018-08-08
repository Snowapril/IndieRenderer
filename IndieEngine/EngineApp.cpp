#include "EngineApp.hpp"
#include "EngineProperty.hpp"
#include "EngineLogger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "EngineProfiler.hpp"
#include "GLResources.hpp"
#include <chrono>

#include "GLDebugger.hpp"

EngineApp::EngineApp()
	: GLApp(), EngineGUI()
{

}

EngineApp::~EngineApp()
{
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gColorSpec);

	glDeleteVertexArrays(1, &simpleQuad);
}

bool EngineApp::init(void)
{
	const auto start = std::chrono::system_clock().now();

	if (!GLApp::init())
		return false;

	if (!buildGeometryBuffers())
		return false;

	if (!buildShaderFiles())
		return false;

	if (!buildUniformBuffers())
		return false;

	//if (!buildFramebuffer())
	//	return false;

	if (!loadTextures())
		return false;
	
	if (!setupLightSources())
		return false;

	if (!initGUI(appWindow))
		return false;

	const auto end = std::chrono::system_clock().now();
	const auto duration = std::chrono::duration<double>(end - start);
	EngineLogger::getConsole()->info("Whole Initialization took {} second.", duration.count());

	return true;
}

void EngineApp::updateScene(float dt)
{
	Profile();
	updateGUI(dt, static_cast<float>(clientHeight));

	//camera.processKeyInput(appWindow, dt);

	camera.onUpdate(dt);
	camera.sendVP(vpUBO, getAspectRatio());
}

void EngineApp::drawScene(void) const
{
	Profile();

	const glm::vec3 viewPos = camera.getViewPos();

	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float time = engineTimer.getTotalTime();

	glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);

	pbrShader->useProgram();

	//Material settings
	materials[materialIdx]->bindMaterial();

	//Transformation settings
	glm::mat4 model;
	model = glm::translate(glm::mat4(), glm::vec3(0.f));
	model = glm::rotate(model, time * rotationVelocity, glm::vec3(0.f, 1.f, 0.f));
	model = glm::scale(model, glm::vec3(scaleRatio));

	pbrShader->sendUniform("model", model);
	renderModels[modelIdx]->drawModel(GL_TRIANGLES);

	for (int i = 0; i < lightPositions.size(); ++i)
	{
		pbrShader->sendUniform("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
		pbrShader->sendUniform("lightColors[" + std::to_string(i) + "]", lightColors[i]);

		model = glm::translate(glm::mat4(), lightPositions[i]);

		pbrShader->sendUniform("model", model);
		renderModels[modelIdx]->drawModel(GL_TRIANGLES);
	}

	pbrShader->sendUniform("viewPos", viewPos);
	
	//pbrShader render settings here
	pbrShader->sendUniform("gamma", gamma);
	pbrShader->sendUniform("useReinhard", useReinhard);
	if (!useReinhard)
		pbrShader->sendUniform("exposure", exposure);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMaps[environmentIdx]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMaps[environmentIdx]);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTextures[environmentIdx]);

	glFrontFace(GL_CW);
	cubeMapShader->useProgram();
	cubeMapShader->sendUniform("gamma", gamma);
	cubeMapShader->sendUniform("exposure", exposure);
	cubeMapShader->sendUniform("useReinhard", useReinhard);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMaps[environmentIdx]);
	
	envCube.drawMesh(GL_TRIANGLES);
	glFrontFace(GL_CCW);

	//unbind
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	renderGUI();
}

bool EngineApp::loadTextures(void)
{
	materials.push_back(std::make_unique<EnginePBRMaterial>("../resources/texture/pbr/grass/"));
	materials.push_back(std::make_unique<EnginePBRMaterial>("../resources/texture/pbr/gold/"));
	materials.push_back(std::make_unique<EnginePBRMaterial>("../resources/texture/pbr/silver/"));
	materials.push_back(std::make_unique<EnginePBRMaterial>("../resources/texture/pbr/rusted_iron/"));
	materials.push_back(std::make_unique<EnginePBRMaterial>("../resources/texture/pbr/plastic/"));
	materials.push_back(std::make_unique<EnginePBRMaterial>("../resources/texture/pbr/wall/"));
	materials.push_back(std::make_unique<EnginePBRMaterial>("../resources/texture/pbr/wood/"));

	if (!buildIBLMap({ "House.hdr",  
					   "Factory.hdr",
					   "Lake.hdr",
					   "City.hdr"}))
		return false;

	return true;
}

bool EngineApp::buildUniformBuffers(void)
{
	glGenBuffers(1, &vpUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, vpUBO, 0, sizeof(glm::mat4) * 2);

	return true;
}

bool EngineApp::setupLightSources(void)
{
	//this is also same setting with learnopengl tutorial

	lightPositions = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	lightColors = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	return true;
}

bool EngineApp::buildGeometryBuffers(void)
{
	renderModels.push_back(std::make_unique<GLModel>("../resources/model/shaderball/shaderball2.obj"));
	renderModels.push_back(std::make_unique<GLModel>("../resources/model/sphere/sphere.obj"));

	return true;
}

bool EngineApp::buildShaderFiles(void)
{
	try 
	{
		deferredShader = std::make_shared<GLShader>("../resources/shader/deferredShader.vert", "../resources/shader/deferredShader.frag", nullptr);
		gBufferShader = std::make_shared<GLShader>("../resources/shader/gBuffer.vert", "../resources/shader/gBuffer.frag", nullptr);
		simpleShader = std::make_shared<GLShader>("../resources/shader/simple_shader.vert", "../resources/shader/simple_shader.frag", nullptr);
		pbrShader = std::make_shared<GLShader>("../resources/shader/pbrShader.vert", "../resources/shader/pbrShader.frag", nullptr);
		cubeMapShader = std::make_shared<GLShader>("../resources/shader/cubeMap.vert", "../resources/shader/cubeMap.frag", nullptr);
	}
	catch (std::exception e)
	{
		EngineLogger::getConsole()->critical("Failed to create shader.");
		return false;
	}

	deferredShader->useProgram();
	deferredShader->sendUniform("gPosition", 0);
	deferredShader->sendUniform("gNormal", 1);
	deferredShader->sendUniform("gColorSpec", 2);

	gBufferShader->useProgram();
	gBufferShader->sendUniform("diffuseTexture", 0);
	gBufferShader->sendUniform("specularTexture", 1);

	pbrShader->useProgram();
	pbrShader->sendUniform("exposure", exposure);
	pbrShader->sendUniform("gamma", gamma);
	pbrShader->sendUniform("albedoMap", 0);
	pbrShader->sendUniform("normalMap", 1);
	pbrShader->sendUniform("metallicMap", 2);
	pbrShader->sendUniform("roughnessMap", 3);
	pbrShader->sendUniform("aoMap", 4);
	pbrShader->sendUniform("irradianceMap", 5);
	pbrShader->sendUniform("prefilterMap", 6);
	pbrShader->sendUniform("brdfLUT", 7);

	cubeMapShader->useProgram();
	cubeMapShader->sendUniform("envMap", 0);

	return true;
}

bool EngineApp::buildFramebuffer(void)
{
	//for post-processing
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, clientWidth, clientHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, clientWidth, clientHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, clientWidth, clientHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	GLuint depthRBO;
	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, clientWidth, clientHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		EngineLogger::getConsole()->critical("Framebuffer is not complete!");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f
	};

	GLuint indices[] = {
		0, 2, 1,
		2, 0, 3
	};

	GLuint VBO, IBO;
	glGenVertexArrays(1, &simpleQuad);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(simpleQuad);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));

	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	return true;
}

bool EngineApp::buildIBLMap(const std::initializer_list<std::string>& pathList)
{
	const int hdrResolution = 512;

	GLShader convertShader("../resources/shader/equiRectangularMapToCubemap.vert", "../resources/shader/equiRectangularMapToCubemap.frag");
	GLShader irradianceMapShader("../resources/shader/equiRectangularMapToCubemap.vert", "../resources/shader/irradianceMap.frag");
	GLShader prefilterShader("../resources/shader/equiRectangularMapToCubemap.vert", "../resources/shader/prefilterMap.frag");
	GLShader brdfShader("../resources/shader/brdf.vert", "../resources/shader/brdf.frag");
	envCube.setupWithFixedGeometryShape(IndieShape::INDIE_BOX);

	//simple quad
	GLfloat vertices[] =
	{
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));

	glBindVertexArray(0);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};

	unsigned int captureFBO, captureRBO;

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	for (const auto& path : pathList)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, hdrResolution, hdrResolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
		
		unsigned int hdrTexture;
		if ((hdrTexture = GLResources::createHDRTexture("../resources/texture/hdr/" + path)) == 0)
		{
			glDeleteRenderbuffers(1, &captureRBO);
			glDeleteFramebuffers(1, &captureFBO);
			return false;
		}

		unsigned int envCubeMap;
		glGenTextures(1, &envCubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, hdrResolution, hdrResolution, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		convertShader.useProgram();
		convertShader.sendUniform("equiRectangularMap", 0);
		convertShader.sendUniform("projection", captureProjection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);

		glViewport(0, 0, hdrResolution, hdrResolution);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		glFrontFace(GL_CW);
		for (unsigned int i = 0; i < 6; ++i)
		{
			convertShader.sendUniform("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubeMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			envCube.drawMesh(GL_TRIANGLES);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glFrontFace(GL_CCW);

		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		unsigned int irradianceMap;
		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		irradianceMapShader.useProgram();
		irradianceMapShader.sendUniform("envMap", 0);
		irradianceMapShader.sendUniform("projection", captureProjection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		glFrontFace(GL_CW);
		for (unsigned int i = 0; i < 6; ++i)
		{
			irradianceMapShader.sendUniform("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			envCube.drawMesh(GL_TRIANGLES);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glFrontFace(GL_CCW);

		unsigned int prefilterMap;
		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		prefilterShader.useProgram();
		prefilterShader.sendUniform("envMap", 0);
		prefilterShader.sendUniform("projection", captureProjection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glFrontFace(GL_CW);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = static_cast<int>(128 * std::pow(0.5, mip));
			unsigned int mipHeight = static_cast<int>(128 * std::pow(0.5, mip));

			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = static_cast<float>(mip) / static_cast<float>(maxMipLevels - 1);
			prefilterShader.sendUniform("roughness", roughness);

			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader.sendUniform("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				envCube.drawMesh(GL_TRIANGLES);
			}
		}
		glFrontFace(GL_CCW);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		unsigned int brdfLUTTexture;
		glGenTextures(1, &brdfLUTTexture);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, hdrResolution, hdrResolution, 0, GL_RG, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, hdrResolution, hdrResolution);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

		glViewport(0, 0, hdrResolution, hdrResolution);
		brdfShader.useProgram();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		envCubeMaps.push_back(envCubeMap);
		irradianceMaps.push_back(irradianceMap);
		prefilterMaps.push_back(prefilterMap);
		brdfLUTTextures.push_back(brdfLUTTexture);
	}
	
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);

	onResize();
	return true;
}

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(appWindow, GLFW_TRUE);	

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		fullscreen = !fullscreen;
		GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* glfwMode = glfwGetVideoMode(glfwMonitor);

		if (fullscreen)
		{
			clientWidth = glfwMode->width;
			clientHeight = glfwMode->height;
			glm::ivec2 startPos(0, 0);

			glfwSetWindowMonitor(appWindow, glfwMonitor, startPos.x, startPos.y, clientWidth, clientHeight, glfwMode->refreshRate);
		}
		else
		{
			clientWidth = CLIENT_WIDTH;
			clientHeight = CLIENT_HEIGHT;
			glm::ivec2 startPos((glfwMode->width - clientWidth) / 2, (glfwMode->height - clientHeight) / 2);

			glfwSetWindowMonitor(appWindow, nullptr, startPos.x, startPos.y, clientWidth, clientHeight, glfwMode->refreshRate);
		}

		onResize();
	}
}

void EngineApp::mousePosCallback(double xpos, double ypos)
{
	//camera.processCursorPos(xpos, ypos);
}

void EngineApp::mouseBtnCallback(int btn, int action, int mods)
{
	camera.processMouseInput(btn, action, mods);
}

void EngineApp::scrollCallback(double xoffset, double yoffset)
{
	camera.processScrollOffset(yoffset);
}

void EngineApp::resizingCallback(int newWidth, int newHeight)
{
	if (newWidth == 0)  newWidth = 1;
	if (newHeight == 0) newHeight = 1;

	clientWidth  = newWidth;
	clientHeight = newHeight;

	onResize();
}