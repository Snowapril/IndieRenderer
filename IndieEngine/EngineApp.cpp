#include "EngineApp.hpp"
#include "EngineProperty.hpp"
#include "EngineLogger.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "EngineProfiler.hpp"


EngineApp::EngineApp()
	: GLApp()
{

}

EngineApp::~EngineApp()
{

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);

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

	testShader->sendUniform("color", glm::vec4(Color::Green[0], Color::Green[1], Color::Green[2], Color::Green[3]));
	testShader->sendUniform("model", model);
	shaderBall.drawModel();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
	testShader = std::make_shared<GLShader>("../resources/shader/vertex_shader.glsl", "../resources/shader/fragment_shader.glsl", nullptr);

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