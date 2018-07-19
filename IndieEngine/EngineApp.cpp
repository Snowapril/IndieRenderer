#include "EngineApp.hpp"
#include "EngineProperty.hpp"
#include "EngineLogger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "EngineProfiler.hpp"
#include "GLResources.hpp"
#include <chrono>

EngineApp::EngineApp()
	: GLApp()
{

}

EngineApp::~EngineApp()
{
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gColorSpec);
	glDeleteTextures(1, &containerDiff);
	glDeleteTextures(1, &containerSpec);

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

	if (!buildFramebuffer())
		return false;

	if (!loadTextures())
		return false;
	
	const auto end = std::chrono::system_clock().now();
	const auto duration = std::chrono::duration<double>(end - start);
	EngineLogger::getConsole()->info("Whole Initialization took {}.", duration.count());

	return true;
}

void EngineApp::updateScene(float dt)
{
	Profile();

	camera.sendVP(vpUBO, getAspectRatio());
}

void EngineApp::drawScene(void) const
{
	Profile();

	const glm::vec3 viewPos = camera.getViewPos();

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glm::mat4 model;
		float time = engineTimer.getTotalTime();

		glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);

		gBufferShader->useProgram();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerDiff);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, containerSpec);

		model = glm::translate(glm::mat4(), glm::vec3(-1.f, 0.f, 0.f));
		model = glm::rotate(model, time, glm::vec3(0.f, 1.f, 0.f));
		gBufferShader->sendUniform("model", model);

		testMesh.drawMesh();

		model = glm::translate(glm::mat4(), glm::vec3(1.f, 0.f, 0.f));
		model = glm::rotate(model, -time, glm::vec3(0.f, 1.f, 0.f));

		gBufferShader->sendUniform("model", model);

		sphere.drawModel();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	deferredShader->useProgram();
	deferredShader->sendUniform("viewPos", viewPos);
	//deferredShader->sendUniform("exposure", exposure);

	//this is same setting with learnopengl tutorial
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		deferredShader->sendUniform("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
		deferredShader->sendUniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		// update attenuation parameters and calculate radius
		const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 0.7f;
		const float quadratic = 1.8f;
		deferredShader->sendUniform("lights[" + std::to_string(i) + "].Linear", linear);
		deferredShader->sendUniform("lights[" + std::to_string(i) + "].Quadratic", quadratic);
	}

	glBindVertexArray(simpleQuad);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool EngineApp::loadTextures(void)
{
	if ((containerDiff = GLResources::createTexture("../resources/texture/container/container2.png", true)) == 0)
		return false;

	if ((containerSpec = GLResources::createTexture("../resources/texture/container/container2_specular.png", true)) == 0)
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

	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 10) / 10.0f) - 5.f;
		float yPos = ((rand() % 10) / 10.0f) - 5.f;
		float zPos = ((rand() % 10) / 10.0f) - 5.f;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	exposure = 1.f;

	return true;
}

bool EngineApp::buildGeometryBuffers(void)
{
	sphere.loadModelFromObjFile("../resources/model/sphere/sphere.obj");

	testMesh.setupWithFixedGeometryShape(IndieShape::INDIE_BOX);

	return true;
}

bool EngineApp::buildShaderFiles(void)
{
	try 
	{
		deferredShader = std::make_shared<GLShader>("../resources/shader/deferredShader.vert", "../resources/shader/deferredShader.frag", nullptr);
		gBufferShader = std::make_shared<GLShader>("../resources/shader/gBuffer.vert", "../resources/shader/gBuffer.frag", nullptr);
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

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(appWindow, GLFW_TRUE);
}

void EngineApp::mousePosCallback(double xpos, double ypos)
{

}

void EngineApp::mouseBtnCallback(int btn, int action, int mods)
{

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