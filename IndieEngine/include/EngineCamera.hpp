/**
* @file		EngineCamera.hpp
* @author	Shinjihong
* @date		17 July 2018
* @version	1.0.0
* @brief	Camera class for IndieEngine.
* @details	update view and projection matrix here and provide them to IndieEngine.
			and also provide view position which is needed for shading.
* @see		
*/

#ifndef ENGINE_CAMERA_HPP
#define ENGINE_CAMERA_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct GLFWwindow; //prototype

class EngineCamera
{
private:
	float fov;

	/// Camera position
	glm::vec3 position;  
	/// Camera direction vector, that is, where player is looking at.
	glm::vec3 direction; 

	bool isFirstCursorMove;  
	glm::vec2 lastCursorPos;

	float mouseSensitivity;
	
	/// degree of freedom. 
	float yaw;   
	/// degree of freedom.
	float pitch; 
	float speed;

	bool toggleZoom;
	bool updateFov;
protected:
public:
	EngineCamera();

	void processMouseInput(int btn, int action, int mods);
	void processCursorPos(double xpos, double ypos);
	void processScrollOffset(double yoffset);
	void processKeyInput(GLFWwindow * window, float dt);

	void onUpdate(float dt);

	/// send view and projection matrix to Uniform Buffer Object.
	void sendVP(unsigned int UBO, float aspectRatio) const; 
	/// return View position vector.
	glm::vec3 getViewPos(void) const; 
};

#endif