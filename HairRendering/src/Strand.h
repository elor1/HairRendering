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
	bool simulate;

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
		mass = 0.01f;
		invMass = 0.0f;
		simulate = true;
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
		mass = 0.01f;
		invMass = 0.0f;
		simulate = true;
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