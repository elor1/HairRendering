#pragma once
#include <vector>
#include <glm.hpp>
#include "Integrator.h"
#include "Mesh.h"

class Hair;

class Simulation
{
public:
	Simulation(Mesh* mesh);
	virtual ~Simulation();

	void Update(float time);
	void Simulate(Hair* hair);

	std::vector<glm::vec3> mForces;
	glm::mat4 GetTransform();

private:
	float mTime;
	Mesh* mMesh;
	glm::mat4 mTransform;
	glm::vec4 mPrevious;

	void Move(Hair* hair);
	void CalculateExternalForces(Hair* hair);
	void CalculateConstraints(Hair* hair);

	void ParticleSimulation(Hair* hair);
};