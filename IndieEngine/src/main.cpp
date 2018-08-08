/** 
* @mainpage		Indie Graphics Engine.
*/

#include <memory>
#include <iostream>
#include "../include/EngineApp.hpp"
#include "../include/EngineLogger.hpp"

/// for optimus feature, in my case HD630? is default and GTX 1060 is external graphics card.
extern "C" 
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

/// Callback function prototypes
static void	error(int error, const char* description);
void localKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void localMousePosCallback(GLFWwindow* window, double xpos, double ypos);
void localMouseBtnCallback(GLFWwindow* window, int btn, int action, int mods);
void localScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void localResizingCallback(GLFWwindow* window, int newWidth, int newHeight);

/// global smart pointer of Engine Application Class.
std::unique_ptr<EngineApp> gEngineApp;

int main(int argc, char* argv[])
{
	gEngineApp = std::make_unique<EngineApp>();
	
	if (!gEngineApp->init()) {
		EngineLogger::getConsole()->critical("Engine App Initialization Failed");
		return -1;
	}
	
	GLFWwindow* appWindow = gEngineApp->getAppWindow();
	glfwSetFramebufferSizeCallback(appWindow, localResizingCallback);
	glfwSetKeyCallback(appWindow, localKeyCallback);
	glfwSetInputMode(appWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetErrorCallback(error);
	glfwSetCursorPosCallback(appWindow, localMousePosCallback);
	glfwSetMouseButtonCallback(appWindow, localMouseBtnCallback);
	glfwSetScrollCallback(appWindow, localScrollCallback);

	return gEngineApp->Run();
}

static void	error(int error, const char* description)
{
	EngineLogger::getConsole()->error(description);
}

void localKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
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

