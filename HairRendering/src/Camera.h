#pragma once
#include <glm.hpp>

class Camera
{
public:
	Camera(float zoom = 5.0f, glm::mat4 projection = glm::mat4(0.0f), float angleX = 0.0f, float angleY = 0.0f);
	glm::mat4 GetView();
	glm::mat4 GetProjection();
	void UpdateViewMatrix();
	void SetAngles(float x = 0.0f, float y = 0.0f);
	void SetPreviousPosition(glm::vec2 pos);
	void SetPreviousRotation(glm::vec2 rotation);
	void SetPrevMousePosition(glm::vec2 pos);
	glm::vec2 GetPreviousPosition();
	glm::vec2 GetPreviousRotation();
	glm::vec2 GetPrevMousePosition();
	float GetZoom();
	float GetAngleX();
	float GetAngleY();
	void SetZoom(float offset);

private:
	glm::mat4 mProjection;
	glm::mat4 mView;
	float mZoom;
	float mAngleX;
	float mAngleY;
	glm::vec2 mPrevPos;
	glm::vec2 mPrevRotation;
	glm::vec2 mPrevMousePos;
};