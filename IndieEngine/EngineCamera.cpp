#include "EngineCamera.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "EngineProperty.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glfw3.h>

EngineCamera::EngineCamera()
	: fov(CAMERA_MAX_FOV), position(0.f, 1.5f, 3.f), isFirstCursorMove(true), speed(CAMERA_SPEED),
		lastCursorPos(0.f, 0.f), yaw(0.f), pitch(0.f), toggleZoom(false), updateFov(false), mouseSensitivity(MOUSE_SENSITIVITY)
{
	direction = glm::normalize(glm::vec3(0.f) - position);
}

void EngineCamera::onUpdate(float dt)
{
	if (!updateFov)
		return;
	
	const float epsilon = 0.1f;

	if (toggleZoom)
	{
		fov -= (fov - CAMERA_MIN_FOV) / (CAMERA_MAX_FOV - CAMERA_MIN_FOV);

		if (fov - epsilon < CAMERA_MIN_FOV) {
			fov = CAMERA_MIN_FOV;
			updateFov = false;
		}
	}
	else
	{
		fov += (CAMERA_MAX_FOV - fov) / (CAMERA_MAX_FOV - CAMERA_MIN_FOV);

		if (fov + epsilon > CAMERA_MAX_FOV) {
			fov = CAMERA_MAX_FOV;
			updateFov = false;
		}
	}
}

void EngineCamera::processMouseInput(int btn, int action, int mods)
{
	if (btn == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		toggleZoom = !toggleZoom;
		updateFov = true;
	}
}

void EngineCamera::processCursorPos(double xpos, double ypos)
{
	if (isFirstCursorMove)
	{
		isFirstCursorMove = false;
		lastCursorPos = glm::vec2(xpos, ypos);
	}

	float xoffset = xpos - lastCursorPos.x;
	float yoffset = lastCursorPos.y - ypos;

	lastCursorPos = glm::vec2(xpos, ypos);

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(front);
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

void EngineCamera::processKeyInput(GLFWwindow * window, float dt)
{
	const float movement = speed * dt;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += direction * movement;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= direction * movement;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
}

void EngineCamera::sendVP(unsigned int UBO, float aspectRatio) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	glm::mat4 view = glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, 1.f, 1000.f);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::vec3 EngineCamera::getViewPos(void) const
{
	return position;
}