#include "EngineApp.hpp"
#include "EngineProperty.hpp"


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

	return true;
}

void EngineApp::updateScene(float dt)
{

}

void EngineApp::drawScene(void) const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);

	//shaderMesh.drawMesh();
}

bool EngineApp::buildGeometryBuffers(void)
{
	shaderBall.loadModelFromObjFile("../resources/model/shaderball/shaderball2.obj");

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

}

void EngineApp::resizingCallback(int newWidth, int newHeight)
{
	if (newWidth == 0)  newWidth = 1;
	if (newHeight == 0) newHeight = 1;

	clientWidth = newWidth;
	clientHeight = newHeight;

	onResize();
}