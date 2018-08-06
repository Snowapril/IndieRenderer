#include "GLApp.hpp"
#include "EngineProperty.hpp"
#include <cassert>
#include <sstream>
#include "EngineLogger.hpp"
#include "GLDebugger.hpp"
#include "EngineProfiler.hpp"

GLApp::GLApp()
	: appWindow(0), appPaused(false), fullscreen(false), minimized(false), maximized(false), resizing(false), enableCulling(true),
	WndCaption("OpenGL Indie Engine Project"), enable4xMsaa(true)
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

	CheckError();

#ifdef _DEBUG
	EngineProfiler::logging("./logs/performance.txt");
#endif

	return 0;
}

void GLApp::onResize(void)
{
	assert(appWindow);

	glViewport(0, 0, clientWidth, clientHeight);
}

bool GLApp::init(void)
{
	if (!initWindow())
		return false;

	if (!initOpenGL())
		return false;

	return true;
}

bool GLApp::initWindow(bool fullscreen)
{
	if (!glfwInit())
	{
		EngineLogger::getConsole()->critical("GLFW Initialization Failed.");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	if (enable4xMsaa) glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_FORWARD_COMPAT);
#endif
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
	GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* glfwMode = glfwGetVideoMode(glfwMonitor);

	glfwWindowHint(GLFW_RED_BITS, glfwMode->redBits);
	glfwWindowHint(GLFW_BLUE_BITS, glfwMode->blueBits);
	glfwWindowHint(GLFW_GREEN_BITS, glfwMode->greenBits);
	glfwWindowHint(GLFW_REFRESH_RATE, glfwMode->refreshRate);

	this->fullscreen = fullscreen;
	clientWidth  = fullscreen ? glfwMode->width : CLIENT_WIDTH;
	clientHeight = fullscreen ? glfwMode->height : CLIENT_HEIGHT;

	appWindow = glfwCreateWindow(clientWidth, clientHeight, WndCaption.c_str(), fullscreen ? glfwMonitor : nullptr , nullptr);

	if (!appWindow) {
		glfwTerminate();
		EngineLogger::getConsole()->critical("GLFW Create Window Failed.");
		return false;
	}
	glfwMakeContextCurrent(appWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		EngineLogger::getConsole()->critical("GLAD Load GLLoader Failed.");
		return false;
	}

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);

	EngineLogger::getConsole()->info("Vendor : {:<15}, Renderer : {:<15}", vendor, renderer);

	return true;
}

bool GLApp::initOpenGL(void)
{
	if (enable4xMsaa)
		glEnable(GL_MULTISAMPLE);

	if (enableCulling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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