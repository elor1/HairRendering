#include "Camera.h"
#include <gtx/transform.hpp>

Camera::Camera(float zoom,  glm::mat4 projection, float angleX, float angleY)
{
	mZoom = zoom;
	mAngleX = angleX;
	mAngleY = angleY;

	/*mView = glm::lookAt(glm::vec3(0.0f, 0.0f, mZoom), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/
	mView = glm::translate(glm::vec3(0.0f, 0.0f, -mZoom)) * glm::rotate(mAngleY, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(mAngleX, glm::vec3(0.0f, 1.0f, 0.0f));
	mProjection = projection;
}

glm::mat4 Camera::GetView() const
{
	return mView;
}

glm::mat4 Camera::GetProjection() const
{
	return mProjection;
}

void Camera::UpdateViewMatrix()
{
	mView = glm::translate(glm::vec3(0.0f, 0.0f, -mZoom)) * glm::rotate(mAngleY, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(mAngleX, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::SetAngles(const float x, const float y)
{
	mAngleX += x;
	mAngleY += y;
}

void Camera::SetPreviousPosition(const glm::vec2 pos)
{
	mPrevPos = pos;
}

void Camera::SetPreviousRotation(const glm::vec2 rotation)
{
	mPrevRotation = rotation;
}

void Camera::SetPrevMousePosition(const glm::vec2 pos)
{
	mPrevMousePos = pos;
}

glm::vec2 Camera::GetPreviousPosition() const
{
	return mPrevPos;
}

glm::vec2 Camera::GetPreviousRotation() const
{
	return mPrevRotation;
}

glm::vec2 Camera::GetPrevMousePosition() const
{
	return mPrevMousePos;
}

float Camera::GetZoom() const
{
	return mZoom;
}

float Camera::GetAngleX() const
{
	return mAngleX;
}

float Camera::GetAngleY() const
{
	return mAngleY;
}

void Camera::SetZoom(const float offset)
{
	mZoom -= offset;
}
