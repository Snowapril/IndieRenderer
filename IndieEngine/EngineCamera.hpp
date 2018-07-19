#ifndef ENGINE_CAMERA_HPP
#define ENGINE_CAMERA_HPP

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class EngineCamera
{
private:
	float fov;

	glm::vec3 position;
	glm::vec3 direction;

	bool isFirstCursorMove;
	glm::vec2 lastCursorPos;

	float yaw;
	float pitch;


protected:
public:
	EngineCamera();

	void processCursorPos(double xpos, double ypos);
	void processScrollOffset(double yoffset);
	void processKeyInput(int key, int action);

	void sendVP(unsigned int UBO, float aspectRatio) const;
	glm::vec3 getViewPos(void) const;
};

#endif