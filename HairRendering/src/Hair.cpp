#include "Hair.h"
#include "Strand.h"
#include "Simulation.h"

Hair::Hair(int numGuides, Simulation* simulation)
{
	for (int i = 0; i < numGuides; i++)
	{
		mGuideHairs.push_back(new Strand(20, 1.0, glm::vec3(i, 0, 0)));
	}

	mSimulation = simulation;
}

void Hair::Update(float time)
{
	/*if (!mSimulation)
	{
		mSimulation->Simulate(this);
	}

	for (auto guide : mGuideHairs)
	{
		guide->Update(time);
	}*/
}

void Hair::Draw(GLuint program)
{
	/*for (auto guide : mGuideHairs)
	{
		guide->Draw(program);
	}*/
}
