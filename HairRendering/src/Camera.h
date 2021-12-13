#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

enum class EMovementDirection
{
	Forward,
	Backward,
	Left,
	Right
};

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

	glm::mat4 GetViewMatrix();
	void Move(EMovementDirection direction, float deltaTime);
	void Rotate(float x, float y);

private:
	glm::vec3 mPosition;
	glm::vec3 mForwards = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;
	float mYaw;
	float mPitch;
	float mMovementSpeed = 0.01f;
	float mSensitivity = 0.1f;

	void UpdateVectors();
};
