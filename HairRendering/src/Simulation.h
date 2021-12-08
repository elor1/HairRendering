#pragma once
#include <vector>
#include <glm.hpp>
#include "Integrator.h"

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
	void Integrate2(Hair* hair);
	void Integrate3(Hair* hair);
	void Integrate4(Hair* hair);
	double OmegaDot(double theta, double omega, float rodLength, float inertia);
};