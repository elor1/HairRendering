#include "Hair.h"
#include "Simulation.h"
#include <SOIL2.h>
#include <iostream>

#define SIMULATE_PHYSICS true

Hair::Hair(int numGuides, Simulation* simulation)
{
	for (int i = 0; i < numGuides; i++)
	{
		mGuideHairs.push_back(new Strand(20, 1.0, glm::vec3(i + 0.25f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	}

	SetAttributes();

	mSimulation = simulation;
}

Hair::Hair(Mesh* mesh, float hairDensity, Simulation* simulation, Hair* oldHair)
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-0.03f, 0.03f);
	for (auto& vertex : mesh->GetVertices())
	{
		glm::vec3 random = glm::vec3(distribution(generator), distribution(generator), distribution(generator));
		glm::vec3 position = vertex.position + random;
		glm::vec3 normal = vertex.normal + random;
		mGuideHairs.push_back(new Strand(20, 0.4, position, normal));
	}

	SetAttributes(oldHair);
	mSimulation = simulation;
}

Hair::Hair(Mesh* mesh, float hairDensity, const char* hairMap, Simulation* simulation, Hair* oldHair)
{
	int width, height, channels;
	unsigned char* image = SOIL_load_image(hairMap, &width, &height, &channels, SOIL_LOAD_RGBA);
	
	if (image == NULL || width == 0 || height == 0)
	{
		std::cout << "ERROR: Failed to load image" << std::endl;
	}

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-0.03f, 0.03f);
	for (auto& vertex : mesh->GetVertices())
	{
		glm::vec3 random = glm::vec3(distribution(generator), distribution(generator), distribution(generator));
		glm::vec2 texCoord = vertex.texCoords + glm::vec2(random.x, random.y);

		//Get uv colour
		int x = vertex.texCoords.x * width;
		int y = ( vertex.texCoords.y) * height;

		/*if (x < 0 || x > width || y < 0 || y > width)
		{
			continue;
		}*/

		unsigned char* pixel = image + y * width * channels + x * channels;
		unsigned char alpha = pixel[3];

		//If pixel alpha is 0, do not place hair
		if ((int)alpha == 0)
		{
			continue;
		}

		//Get length from alpha value
		double length = (double)alpha / 637.5;
		glm::vec3 position = vertex.position + random;
		glm::vec3 normal = vertex.normal + random;

		mGuideHairs.push_back(new Strand(20, length, position, normal));
	}

	SOIL_free_image_data(image);
	SetAttributes(oldHair);
	mSimulation = simulation;
}

void Hair::Update(float time)
{
	if (SIMULATE_PHYSICS && mSimulation)
	{
		mSimulation->Simulate(this);
	}

	for (auto& guide : mGuideHairs)
	{
		guide->Update(time);
	}
}

void Hair::Draw(ShaderProgram* program)
{
	program->uniforms.colour = mColour;
	program->uniforms.numGroupHairs = mNumGroupHairs;
	program->uniforms.groupSpread = mGroupSpread;
	program->uniforms.hairRadius = mHairRadius;
	program->uniforms.noiseAmplitude = mNoiseAmplitude;
	program->uniforms.numSplineVertices = mNumSplineVertices;
	program->SetObjectUniforms();

	for (auto& guide : mGuideHairs)
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
		SetAttributes(oldHair->mColour, oldHair->mNumGroupHairs, oldHair->mGroupSpread, oldHair->mHairRadius, oldHair->mNoiseAmplitude, oldHair->mNumSplineVertices);
	}
}

void Hair::SetAttributes(glm::vec3 colour, int numGroupHairs, float groupSpread, float hairRadius, float noiseAmplitude, int numSplineVertices)
{
	mColour = colour;
	mNumGroupHairs = numGroupHairs;
	mGroupSpread = groupSpread;
	mHairRadius = hairRadius;
	mNoiseAmplitude = noiseAmplitude;
	mNumSplineVertices = numSplineVertices;
}

std::vector<Strand*> Hair::GetGuideHairs()
{
	return mGuideHairs;
}

int Hair::GetNumGroupHairs()
{
	return mNumGroupHairs;
}

void Hair::SetNumGroupHairs(int num)
{
	mNumGroupHairs = num;
}

void Hair::SetNumSplineVertices(int num)
{
	mNumSplineVertices = num;
}

void Hair::SetColour(glm::vec3 colour)
{
	mColour = colour;
}
