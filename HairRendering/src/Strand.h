#pragma once
#include <glm.hpp>
#include "Shape.h"
#include <vector>
#include "ShaderProgram.h"

struct HairVertex
{
	glm::vec3 position;
	glm::vec3 tempPosition;
	glm::vec3 velocity;
	glm::vec3 forces;
	glm::vec3 correction;
	double theta;
	double omega;
	float segmentLength;
	float mass;
	float invMass;

	HairVertex()
	{
		position = glm::vec3(0.0f);
		tempPosition = glm::vec3(0.0f);
		velocity = glm::vec3(0.0f);
		forces = glm::vec3(0.0f);
		theta = 0.0;
		omega = 0.0;
		segmentLength = 0.0f;
		correction = glm::vec3(0.0f);
		mass = 0.0f;
		invMass = 0.0f;
	}

	HairVertex(glm::vec3 pos)
	{
		position = pos;
		tempPosition = pos;
		velocity = glm::vec3(0.0f);
		forces = glm::vec3(0.0f);
		theta = 0.0;
		omega = 0.0;
		segmentLength = 0.0;
		correction = glm::vec3(0.0f);
		mass = 0.0f;
		invMass = 0.0f;
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
	Strand(int numSegments, double length, glm::vec3 position, glm::vec3 direction);
	virtual ~Strand();

	void Update(float time);
	void Draw(ShaderProgram* program);

	std::vector<HairVertex*> mVertices;
	Shape mPatch;
	int mNumSegments;
	double mLength;
	glm::vec3 mTriangleFace[2];
};