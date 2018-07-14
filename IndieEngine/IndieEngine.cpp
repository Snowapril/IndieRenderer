#include "GLApp.hpp"
#include "EngineProperty.hpp"
#include <memory>
#include <iostream>

extern "C" 
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

class EngineApp : public GLApp
{
private:
	bool initCallback(void);
protected:
public:
	EngineApp();
	~EngineApp();

	virtual bool init(void);

	virtual void updateScene(float dt);
	virtual void drawScene(void) const;

	virtual void keyCallback(int key, int scancode, int action, int mode) {};
	virtual void mousePosCallback(double xpos, double ypos) {};
	virtual void mouseBtnCallback(int btn, int action, int mods) {};
	virtual void scrollCallback(double xoffset, double yoffset) {};
	virtual void resizingCallback(int newWidth, int newHeight) {};
};

//global smart pointer of Engine Application Class.
std::unique_ptr<EngineApp> gEngineApp;

int main(void)
{
	gEngineApp = std::make_unique<EngineApp>();

	if (!gEngineApp->init()) {
		std::cerr << "Engine App Initialization Failed." << std::endl;
		return -1;
	}

	return gEngineApp->Run();
}

void EngineApp::updateScene(float dt)
{

}

void EngineApp::drawScene(void) const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(Color::Blue[0], Color::Blue[1], Color::Blue[2], Color::Blue[3]);
}

//Callback function prototypes
void localKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) ;
void localMousePosCallback(GLFWwindow* window, double xpos, double ypos) ;
void localMouseBtnCallback(GLFWwindow* window, int btn, int action, int mods) ;
void localScrollCallback(GLFWwindow* window, double xoffset, double yoffset) ;
void localResizingCallback(GLFWwindow* window, int newWidth, int newHeight) ;

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

	if (!initCallback())
		return false;

	return true;
}

bool EngineApp::initCallback(void)
{
	glfwSetFramebufferSizeCallback(appWindow, localResizingCallback);
	glfwSetKeyCallback(appWindow, localKeyCallback);
	glfwSetCursorPosCallback(appWindow, localMousePosCallback);
	glfwSetMouseButtonCallback(appWindow, localMouseBtnCallback);
	glfwSetScrollCallback(appWindow, localScrollCallback);

	return true;
}



void localKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	gEngineApp->keyCallback(key, scancode, action, mode);
}

void localMousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	gEngineApp->mousePosCallback(xpos, ypos);
}

void localMouseBtnCallback(GLFWwindow* window, int btn, int action, int mods)
{
	gEngineApp->mouseBtnCallback(btn, action, mods);
}

void localScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gEngineApp->scrollCallback(xoffset, yoffset);
}

void localResizingCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	gEngineApp->resizingCallback(newWidth, newHeight);
}