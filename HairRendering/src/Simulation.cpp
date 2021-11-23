#include "Simulation.h"
#include "Hair.h"
#include "Strand.h"

Simulation::Simulation()
{
}

Simulation::~Simulation()
{
}

void Simulation::Update(float time)
{
	mTime = time;
}

void Simulation::Simulate(Hair* hair)
{
	CalculateExternalForces(hair);
	CalculateConstraints(hair);
	Integrate(hair);
}

void Simulation::CalculateExternalForces(Hair* hair)
{
	for (auto guide : hair->mGuideHairs)
	{
		for (auto joint : guide->mJoints)
		{
			joint->linear = glm::vec3(0.0f);
			joint->angular = glm::vec3(0.0f);
		}
	}
}

void Simulation::CalculateConstraints(Hair* hair)
{
	for (auto guide : hair->mGuideHairs)
	{
		for (auto joint : guide->mJoints)
		{
			joint->constraint = glm::vec3(0.0f);
		}
	}
}

void Simulation::Integrate(Hair* hair)
{
	for (auto guide : hair->mGuideHairs)
	{
		float numVertices = guide->mVertices.size();
		for (int i = 0; i < numVertices; i++)
		{
			HairVertex* vertex = guide->mVertices[i];
			vertex->position.x = 0.0f * sin(8 * (mTime + (float)i / numVertices));
		}
	}
}
