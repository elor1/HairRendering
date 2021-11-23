#pragma once
#include <glm.hpp>

struct HairVertex
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	HairVertex()
	{
		position = glm::vec3(0.0f);
		velocity = glm::vec3(0.0f);
		acceleration = glm::vec3(0.0f);
	}

	HairVertex(glm::vec3 pos)
	{
		position = pos;
		velocity = glm::vec3(0.0f);
		acceleration = glm::vec3(0.0f);
	}

	HairVertex(glm::vec3 p, glm::vec3 v, glm::vec3 a)
	{
		position = p;
		velocity = v;
		acceleration = a;
	}
};

class Hair
{
	Hair(int numSegments, double length, glm::vec3 position);
	virtual ~Hair();

	int GetNumSegments();
	double GetLength();
	HairVertex GetVertex(int index);
	HairVertex* GetVertices();

private:
	int mNumSegments;
	double mLength;
	HairVertex* mVertices;
};