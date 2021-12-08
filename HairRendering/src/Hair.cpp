#include "Hair.h"
#include "Strand.h"
#include "Simulation.h"
#include <SOIL2.h>
#include <iostream>

Hair::Hair(int numGuides, Simulation* simulation)
{
	for (int i = 0; i < numGuides; i++)
	{
		mGuideHairs.push_back(new Strand(3, 1.0, glm::vec3(i + 0.25f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	}

	SetAttributes();

	mSimulation = simulation;
}

Hair::Hair(Hair* oldHair, Simulation* simulation)
{
	mNumGuideHairs = oldHair->mNumGuideHairs;

	for (int i = 0; i < mNumGuideHairs; i++)
	{
		mGuideHairs.push_back(new Strand(3, 1.0, glm::vec3(i + 0.25f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	}

	SetAttributes(oldHair);

	mSimulation = simulation;
}

Hair::Hair(Mesh* mesh, float hairDensity, const char* hairTexture, Simulation* simulation, Hair* oldHair/*, int width, int height*/)
{
	int width, height;
	unsigned char* image = SOIL_load_image(hairTexture, &width, &height, NULL, SOIL_LOAD_RGB);

	for (auto triangle : mesh->triangles)
	{
		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
		int numHairs = (int)(hairDensity * triangle.Area() + distribution(generator));
		for (int i = 0; i < numHairs; i++)
		{
			glm::vec3 position;
			glm::vec2 texCoord;
			glm::vec3 normal;
			triangle.RandomPoint(position, texCoord, normal);

			int x = texCoord.x * width;
			int y = (1 - texCoord.y) * height;
			if (x < 0 || x > width || y < 0 || y > height)
			{
				continue;
			}
			unsigned char* colour = image + y * width * 3 + x * 3;
			//std::cout << (int)colour[0] << ", " << (int)colour[1] << ", " << (int)colour[2] << std::endl;
			if (colour[0] == 0 && colour[1] == 0 && colour[2] == 0)
			{
				continue;
			}

			normal.z = 0.0f;
			normal = glm::normalize(normal);
			mGuideHairs.push_back(new Strand(3, 1, position, normal));
		}
	}

	SetAttributes(oldHair);
	mSimulation = simulation;

	SOIL_free_image_data(image);
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
	program.uniforms.colour = mColour;
	program.uniforms.numGroupHairs = mNumGroupHairs;
	program.uniforms.groupWidth = mGroupWidth;
	program.uniforms.hairRadius = mHairRadius;
	program.uniforms.noiseAmplitude = mNoiseAmplitude;
	program.uniforms.numSplineVertices = mNumSplineVertices;
	program.SetHairUniforms();

	for (auto guide : mGuideHairs)
	{
		guide->Draw(program);
	}
}

void Hair::SetAttributes(Hair* oldHair)
{
	if (!oldHair)
	{
		SetAttributes();
	}
	else
	{
		SetAttributes(oldHair->mColour, oldHair->mNumGroupHairs, oldHair->mGroupWidth, oldHair->mHairRadius, oldHair->mNoiseAmplitude, oldHair->mNumSplineVertices);
	}
}

void Hair::SetAttributes(glm::vec3 colour, int numGroupHairs, float groupWidth, float hairRadius, float noiseAmplitude, int numSplineVertices)
{
	mColour = colour;
	mNumGroupHairs = numGroupHairs;
	mGroupWidth = groupWidth;
	mHairRadius = hairRadius;
	mNoiseAmplitude = noiseAmplitude;
	mNumSplineVertices = numSplineVertices;
}
