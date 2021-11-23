#include "Hair.h"
#include "Strand.h"
#include "Simulation.h"

Hair::Hair(int numGuides, Simulation* simulation)
{
	for (int i = 0; i < numGuides; i++)
	{
		mGuideHairs.push_back(new Strand(2, 2.0, glm::vec3(i + 0.25f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	}

	mSimulation = simulation;
}

void Hair::Update(float time)
{
	if (!mSimulation)
	{
		mSimulation->Simulate(this);
	}

	for (auto guide : mGuideHairs)
	{
		guide->Update(time);
	}
}

void Hair::Draw(ShaderProgram &program)
{
	program.uniforms.colour = glm::vec3(0.6f, 0.4f, 0.3f);
	program.uniforms.numGroupHairs = 15;
	program.uniforms.groupWidth = 0.2f;
	program.uniforms.hairRadius = 0.005f;
	program.uniforms.noiseAmplitude = 0.03f;
	program.uniforms.numSplineVertices = 20;
	program.SetHairUniforms();

	for (auto guide : mGuideHairs)
	{
		guide->Draw(program);
	}
}
