#pragma once
#include <glm.hpp>
#include "Shape.h"
#include <vector>
#include "ShaderProgram.h"

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

struct Joint
{
	glm::vec3 position;
	glm::vec3 linear;
	glm::vec3 angular;
	glm::vec3 constraint;

	Joint()
	{
		position = glm::vec3(0.0f);
		linear = glm::vec3(0.0f);
		angular = glm::vec3(0.0f);
		constraint = glm::vec3(0.0f);
	}

	Joint(glm::vec3 pos)
	{
		position = pos;
		linear = glm::vec3(0.0f);
		angular = glm::vec3(0.0f);
		constraint = glm::vec3(0.0f);
	}
};

class Strand
{
public:
	Strand(int numSegments, double length, glm::vec3 position);
	virtual ~Strand();

	void Update(float time);
	void Draw(ShaderProgram &program);

	std::vector<HairVertex*> mVertices;
	std::vector<Joint*> mJoints;
	Shape mPatch;
	int mNumSegments;
	double mLength;

};