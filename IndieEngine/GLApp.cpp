#include "GLApp.hpp"
#include "EngineProperty.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

GLApp::GLApp()
	: appWindow(0), appPaused(false), minimized(false), maximized(false), resizing(false), 
	WndCaption("OpenGL Indie Engine Project"), clientWidth(CLIENT_WIDTH), clientHeight(CLIENT_HEIGHT), enable4xMsaa(false)
{
}

GLApp::~GLApp()
{
	glfwTerminate();
}

int GLApp::Run(void)
{
	engineTimer.reset();

	while (!glfwWindowShouldClose(appWindow))
	{
		engineTimer.tick();

		if (!appPaused)
		{
			calculateFrameStats();
			updateScene(engineTimer.getDeltaTime());
			drawScene();
		}
		else
		{
			Sleep(100);
		}

		glfwSwapBuffers(appWindow);
		glfwPollEvents();
	}

	return 0;
}

void GLApp::onResize(void)
{
	assert(appWindow);


}

bool GLApp::init(void)
{
	if (!initWindow())
		return false;

	if (!initOpenGL())
		return false;

	return true;
}

bool GLApp::initWindow(void)
{
	if (!glfwInit())
	{
		std::cerr << "GLFW Initialization Failed." << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	if (enable4xMsaa)
		glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_FORWARD_COMPAT);
#endif

	appWindow = glfwCreateWindow(clientWidth, clientHeight, WndCaption.c_str(), nullptr, nullptr);
	if (!appWindow) {
		glfwTerminate();
		std::cerr << "GLFW Create Window Failed." << std::endl;
		return false;
	}
	glfwMakeContextCurrent(appWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		std::cerr << "GLAD Load GLLoader Failed." << std::endl;
		return false;
	}

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);

	std::clog << vendor << std::endl;
	std::clog << renderer << std::endl;

	return true;
}

bool GLApp::initOpenGL(void)
{
	if (enable4xMsaa)
		glEnable(GL_MULTISAMPLE);

	return true;
}

void GLApp::calculateFrameStats(void)
{
	static int frameCnts = 0;
	static float timeElapsed = 0.0f;

	frameCnts++;

	if (engineTimer.getTotalTime() - timeElapsed >= 1.0f)
	{
		float fps = static_cast<float>(frameCnts);
		float mspf = 1000.f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << WndCaption     << "   "
			 << "FPS: " << fps << "   "
			 << "Frame Time: " << mspf << " (ms)";

		glfwSetWindowTitle(appWindow, outs.str().c_str());

		frameCnts = 0;
		timeElapsed += 1.f;
	}
}