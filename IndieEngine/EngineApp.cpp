#include "EngineApp.hpp"
#include "EngineProperty.hpp"
#include "EngineLogger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "EngineProfiler.hpp"
#include "GLResources.hpp"


EngineApp::EngineApp()
	: GLApp()
{

}

EngineApp::~EngineApp()
{
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &colorBufferTexture);
	glDeleteRenderbuffers(1, &depthStencilRBO);

	glDeleteVertexArrays(1, &simpleQuad);
}

bool EngineApp::init(void)
{
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

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glm::mat4 model;
	float time = engineTimer.getTotalTime();

	glBindBuffer(GL_UNIFORM_BUFFER, vpUBO);

	testShader->useProgram();

	model = glm::translate(glm::mat4(), glm::vec3(-1.f, 0.f, 0.f));
	model = glm::rotate(model, time, glm::vec3(0.f, 1.f, 0.f));

	testShader->sendUniform("model", model);
	testShader->sendUniform("color", glm::vec4(Color::Cyan[0], Color::Cyan[1], Color::Cyan[2], Color::Cyan[3]));
	testMesh.drawMesh();

	model = glm::rotate(model, - time, glm::vec3(0.f, 1.f, 0.f));
	model = glm::translate(glm::mat4(), glm::vec3(1.f, 0.f, 0.f));


	const std::vector<GLMesh> &meshes = shaderBall.meshes;
	for (int i = 0; i < meshes.size(); ++i)
	{
		testShader->sendUniform("color", glm::vec4(0.15f * (i + 1), 0.15f * (i + 1), 0.15f * (i + 1), 1.f));
		testShader->sendUniform("model", model);

		meshes[i].drawMesh();
	}

	shaderBall.drawModel();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	postProcessing->useProgram();
	glBindVertexArray(simpleQuad);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBufferTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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

bool EngineApp::buildGeometryBuffers(void)
{
	shaderBall.loadModelFromObjFile("../resources/model/shaderball/shaderball2.obj");

	testMesh.setupWithFixedGeometryShape(IndieShape::INDIE_BOX);

	return true;
}

bool EngineApp::buildShaderFiles(void)
{
	testShader     = std::make_shared<GLShader>("../resources/shader/simple_shader.vert",   "../resources/shader/simple_shader.frag",   nullptr);
	postProcessing = std::make_shared<GLShader>("../resources/shader/post_processing.vert", "../resources/shader/post_processing.frag", nullptr);
	
	postProcessing->useProgram();
	postProcessing->sendUniform("screenTexture", 0);

	return true;
}

bool EngineApp::buildFramebuffer(void)
{
	//for post-processing
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	colorBufferTexture = GLResources::createColorbuffer(clientWidth, clientHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferTexture, 0);

	depthStencilRBO = GLResources::createDepthStencilRBO(clientWidth, clientHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilRBO);

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