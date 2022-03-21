#pragma once
#include <vector>
#include <glm.hpp>
#include "Mesh.h"
#include <map>
#include <stdexcept>
#include <thread>

class Hair;
class Strand;

class Simulation
{
public:
	Simulation(Mesh* mesh);

	void Update(float time);
	void Simulate(Hair* hair);
	void UpdateRotation(Hair* hair, float angle, glm::vec3 axis);
	void UpdatePosition(Hair* hair, glm::vec3 transform);
	void UpdateHair(Hair* hair);

	std::vector<glm::vec3> mForces;
	glm::mat4 GetTransform() const;
	void SetHeadMoving(const bool moving);

	//Gui
	bool shake;
	bool nod;
	glm::vec3 windDirection;
	float windStrength;
	float stiffness;
	float dampening;

	void ResetPosition();

private:
	float mTime;
	Mesh* mMesh;
	glm::mat4 mTransform;
	bool mHeadMoving;

	void Move(Hair* hair);
	void CalculateExternalForces(Hair* hair);

	void ParticleSimulation(Hair* hair);
};
