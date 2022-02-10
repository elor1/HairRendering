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

glm::mat4 Camera::GetView()
{
	return mView;
}

glm::mat4 Camera::GetProjection()
{
	return mProjection;
}

void Camera::UpdateViewMatrix()
{
	mView = glm::translate(glm::vec3(0.0f, 0.0f, -mZoom)) * glm::rotate(mAngleY, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(mAngleX, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::SetAngles(float x, float y)
{
	mAngleX += x;
	mAngleY += y;
}

void Camera::SetPreviousPosition(glm::vec2 pos)
{
	mPrevPos = pos;
}

void Camera::SetPreviousRotation(glm::vec2 rotation)
{
	mPrevRotation = rotation;
}

void Camera::SetPrevMousePosition(glm::vec2 pos)
{
	mPrevMousePos = pos;
}

glm::vec2 Camera::GetPreviousPosition()
{
	return mPrevPos;
}

glm::vec2 Camera::GetPreviousRotation()
{
	return mPrevRotation;
}

glm::vec2 Camera::GetPrevMousePosition()
{
	return mPrevMousePos;
}

float Camera::GetZoom()
{
	return mZoom;
}

float Camera::GetAngleX()
{
	return mAngleX;
}

float Camera::GetAngleY()
{
	return mAngleY;
}

void Camera::SetZoom(float offset)
{
	mZoom -= offset;
}
