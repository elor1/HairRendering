#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
{
	mPosition = position;
	mWorldUp = worldUp;
	mYaw = yaw;
	mPitch = pitch;

	UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(mPosition, mPosition + mForwards, mUp);
}

void Camera::Move(EMovementDirection direction, float deltaTime)
{
	float speed = mMovementSpeed * deltaTime;

	if (direction == EMovementDirection::Forward)
	{
		mPosition += mForwards * speed;
	}

	if (direction == EMovementDirection::Backward)
	{
		mPosition -= mForwards * speed;
	}

	if (direction == EMovementDirection::Left)
	{
		mPosition -= mRight * speed;
	}

	if (direction == EMovementDirection::Right)
	{
		mPosition += mRight * speed;
	}

	if (direction == EMovementDirection::Up)
	{
		mPosition += mUp * speed;
	}

	if (direction == EMovementDirection::Down)
	{
		mPosition -= mUp * speed;
	}
}

void Camera::Rotate(float x, float y)
{
	x *= mSensitivity;
	y *= mSensitivity;

	mYaw += x;
	mPitch += y;

	if (mPitch > 90.0f)
	{
		mPitch = 90.0f;
	}

	if (mPitch < -90.0f)
	{
		mPitch = -90.0f;
	}

	UpdateVectors();
}

void Camera::UpdateVectors()
{
	glm::vec3 forwards = glm::vec3(cos(glm::radians(mYaw)) * cos(glm::radians(mPitch)),
		sin(glm::radians(mPitch)),
		sin(glm::radians(mYaw)) * cos(glm::radians(mPitch)));
	mForwards = glm::normalize(forwards);
	mRight = glm::normalize(glm::cross(mForwards, mWorldUp));
	mUp = glm::normalize(glm::cross(mRight, mForwards));
}