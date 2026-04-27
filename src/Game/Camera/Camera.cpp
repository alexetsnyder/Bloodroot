#include "Camera.h"

namespace Game
{
	Camera::Camera(glm::vec3 position, glm::vec3 worldUp)
		: position(position), worldUp(worldUp), yaw(DEFAULT_YAW), pitch(DEFAULT_PITCH),
		  movementSpeed(DEFAULT_SPEED), mouseSensitivity(DEFAULT_SENSITIVITY), zoom(DEFAULT_ZOOM)
	{
		updateCameraVectors();
	}

	void Camera::processKeyboard(CameraMovement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == CameraMovement::FORWARD)
		{
			position += front * velocity;
		}
		if (direction == CameraMovement::BACKWARD)
		{
			position -= front * velocity;
		}
		if (direction == CameraMovement::LEFT)
		{
			position -= right * velocity;
		}
		if (direction == CameraMovement::RIGHT)
		{
			position += right * velocity;
		}
	}

	void Camera::processMouseMovement(float xOffset, float yOffset)
	{
		xOffset *= mouseSensitivity;
		yOffset += mouseSensitivity;

		yaw += xOffset;
		pitch += yOffset;

		pitch = std::min(89.0f, std::max(-89.0f, pitch));

		updateCameraVectors();
	}

	void Camera::processMouseScroll(float yOffset)
	{
		zoom -= yOffset;
		zoom = std::min(45.0f, std::max(1.0f, zoom));
	}

	void Camera::updateCameraVectors()
	{
		front = glm::normalize(
			glm::vec3
			{
				cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
				sin(glm::radians(yaw)),
				sin(glm::radians(yaw)) * cos(glm::radians(pitch))
			});

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
}