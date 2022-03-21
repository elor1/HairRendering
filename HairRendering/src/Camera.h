#pragma once
#include <glm.hpp>

class Camera
{
public:
	Camera(float zoom = 5.0f, glm::mat4 projection = glm::mat4(0.0f), float angleX = 0.0f, float angleY = 0.0f);
	glm::mat4 GetView() const;
	glm::mat4 GetProjection() const;
	void UpdateViewMatrix();
	void SetAngles(const float x = 0.0f, const float y = 0.0f);
	void SetPreviousPosition(const glm::vec2 pos);
	void SetPreviousRotation(const glm::vec2 rotation);
	void SetPrevMousePosition(const glm::vec2 pos);
	glm::vec2 GetPreviousPosition() const;
	glm::vec2 GetPreviousRotation() const;
	glm::vec2 GetPrevMousePosition() const;
	float GetZoom() const;
	float GetAngleX() const;
	float GetAngleY() const;
	void SetZoom(const float offset);

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