#ifndef GLAPP_HPP
#define GLAPP_HPP

#include <glad/glad.h>
#include <GL/glfw3.h>
#include <string>
#include "EngineTimer.hpp"

class GLApp
{
protected:
	GLFWwindow * appWindow;

	std::string WndCaption;

	int clientWidth;
	int clientHeight;

	bool enable4xMsaa;
	bool enableCulling;

	bool maximized;
	bool minimized;
	bool resizing;

	bool appPaused;

	EngineTimer engineTimer;
protected:
	bool initWindow(void);
	bool initOpenGL(void);

	void calculateFrameStats(void);
public:
	GLApp();
	~GLApp();

	float getAspectRatio(void) const { return static_cast<float>(clientWidth) / clientHeight; }
	GLFWwindow* getAppWindow(void) const { return appWindow; }

	int Run(void);

	virtual bool init(void);
	virtual void onResize(void);

	
	virtual void updateScene(float dt) = 0;
	virtual void drawScene(void) const = 0;

	virtual void keyCallback(int key, int scancode, int action, int mode) {};
	virtual void mousePosCallback(double xpos, double ypos) {};
	virtual void mouseBtnCallback(int btn, int action, int mods) {};
	virtual void scrollCallback(double xoffset, double yoffset) {};
	virtual void resizingCallback(int newWidth, int newHeight) {};
};


#endif