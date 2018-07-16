#include "EngineCamera.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "EngineProperty.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glfw3.h>

EngineCamera::EngineCamera()
	: fov(CAMERA_FOV), position(0.f, 2.5f, 2.5f), direction(0.f), isFirstCursorMove(true), lastCursorPos(0.f, 0.f), yaw(0.f), pitch(0.f)
{
}

void EngineCamera::processCursorPos(double xpos, double ypos)
{

}

void EngineCamera::processScrollOffset(double yoffset)
{
	if (fov >= CAMERA_MIN_FOV && fov <= CAMERA_MAX_FOV)
		fov -= static_cast<float>(yoffset);
	if (fov < CAMERA_MIN_FOV)
		fov = CAMERA_MIN_FOV;
	if (fov > CAMERA_MAX_FOV)
		fov = CAMERA_MAX_FOV;
}

void EngineCamera::processKeyInput(int key, int action)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS);
	if (key == GLFW_KEY_A && action == GLFW_PRESS);
	if (key == GLFW_KEY_S && action == GLFW_PRESS);
	if (key == GLFW_KEY_D && action == GLFW_PRESS);
}

void EngineCamera::sendVP(unsigned int UBO, float aspectRatio) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	glm::mat4 view = glm::lookAt(position, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, 1.f, 1000.f);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}