#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Game
{
	enum class CameraMovement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
	};

	class Camera
	{
		public:
			const float DEFAULT_YAW = -90.0f;
			const float DEFAULT_PITCH = 0.0f;
			const float DEFAULT_SPEED = 2.5f;
			const float DEFAULT_SENSITIVITY = 0.1f;
			const float DEFAULT_ZOOM = 45.0f;

			Camera(glm::vec3 position, glm::vec3 worldUp);

			glm::mat4 getViewMatrix() { return glm::lookAt(position, position + front, up); }
			glm::vec3 Position() { return position; }

			void processKeyboard(CameraMovement direction, float deltaTime);
			void processMouseMovement(float xOffset, float yOffset);
			void processMouseScroll(float yOffset);

		private:
			glm::vec3 position;
			glm::vec3 front;
			glm::vec3 right;
			glm::vec3 up;
			glm::vec3 worldUp;
			float yaw, pitch;
			float movementSpeed;
			float mouseSensitivity;
			float zoom;

			void updateCameraVectors();
	};
}