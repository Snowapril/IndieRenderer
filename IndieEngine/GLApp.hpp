/**
* @file		GLApp.hpp
* @author	Shinjihong
* @date		15 July 2018
* @version	1.0.0
* @brief	OpenGL Window settings.
* @details	Initialize OpenGL Window and provide all virtual function for inheritance.
* @see
*/

#ifndef GLAPP_HPP
#define GLAPP_HPP

#include <glad/glad.h>
#include <GL/glfw3.h>
#include <string>
#include "EngineTimer.hpp"

class GLApp
{
protected:
	/// OpenGL Window
	GLFWwindow * appWindow;  
	/// OpenGL Window Title.
	std::string WndCaption;  
	/// OpenGL Window Width
	int clientWidth;  
	/// OpenGL Window Height
	int clientHeight; 
	/// provide multi sampling or not.
	bool enable4xMsaa;  
	/// provide culling face or not.
	bool enableCulling; 

	/// is window maximized or not
	bool maximized; 
	/// is window minimized or not
	bool minimized; 
	/// is window resizing or not
	bool resizing;  
	/// is window paused or not
	bool appPaused; 
	/// provide time to Engine. delta time and total time can be provided.
	EngineTimer engineTimer; 
protected:
	/// initialize OpenGL Window.
	bool initWindow(void);
	/// initilaize OpenGL setting, for example multisampling, culling face, depth test i.e
	bool initOpenGL(void);
	/// calculate FPS and frame time.
	void calculateFrameStats(void); 
public:
	GLApp();
	virtual ~GLApp();

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