#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 colour) : Model("../models/sphere.obj")
{
	mPosition = position;
	mColour = colour;
	mOriginalPos = GetFirstMesh()->GetVertices();
}

void Light::SetPosition(glm::vec3 pos)
{
	std::vector<Vertex> newPos = mOriginalPos;
	for (int i = 0; i < mOriginalPos.size(); i++)
	{
		newPos[i].position = mOriginalPos[i].position + pos;
	}

	GetFirstMesh()->SetVertices(newPos);
}

glm::vec3 Light::GetColour()
{
	return mColour;
}
