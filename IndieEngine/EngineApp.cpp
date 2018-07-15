#include "EngineApp.hpp"
#include "EngineProperty.hpp"


void EngineApp::updateScene(float dt)
{

}

void EngineApp::drawScene(void) const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);
}



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

	return true;
}

void EngineApp::keyCallback(int key, int scancode, int action, int mode)
{

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