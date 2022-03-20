#pragma once
#include <glm.hpp>
#include "Model.h"

class Light : public Model
{
public:
	Light(glm::vec3 position, glm::vec3 colour);

	void SetPosition(glm::vec3 pos);
	glm::vec3 GetColour();

private:
	glm::vec3 mPosition;
	glm::vec3 mColour;
	std::vector<Vertex> mOriginalPos;
};