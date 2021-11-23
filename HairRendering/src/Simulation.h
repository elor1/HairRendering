#pragma once
#include <vector>
#include <glm.hpp>

class Hair;

class Simulation
{
public:
	Simulation();
	virtual ~Simulation();

	void Update(float time);
	void Simulate(Hair* hair);

	std::vector<glm::vec3> mForces;

private:
	float mTime;

	void CalculateExternalForces(Hair* hair);
	void CalculateConstraints(Hair* hair);
	void Integrate(Hair* hair);
};