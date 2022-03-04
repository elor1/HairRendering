#include "Hair.h"
#include "Simulation.h"
#include <SOIL2.h>
#include <iostream>
#include "Texture.h"

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

Hair::Hair(Mesh* mesh, float hairDensity, const char* hairMap, double maxLength, Simulation* simulation, Hair* oldHair)
{
	if (hairMap == "")
	{
		Hair(mesh, hairDensity, simulation, oldHair);
		return;
	}
	int width, height, channels;
	unsigned char* image = SOIL_load_image(hairMap, &width, &height, &channels, SOIL_LOAD_RGBA);
	
	if (image == NULL || width == 0 || height == 0)
	{
		std::cout << "ERROR: Failed to load image" << std::endl;
	}

	mHairMap = new Texture();
	mHairMap->Create(hairMap, GL_LINEAR, GL_LINEAR);

	for (auto& triangle : mesh->triangles)
	{
		int numHairs = (int)(hairDensity * triangle.Area() + rand() / (float)RAND_MAX);
		for (int i = 0; i < numHairs; i++)
		{
			Vertex randomPoint;
			triangle.RandomPoint(randomPoint);
			randomPoint.texCoords = glm::vec2(glm::min(randomPoint.texCoords.x, 0.999f), glm::min(randomPoint.texCoords.y, 0.999f));

			int x = randomPoint.texCoords.x * width;
			int y = randomPoint.texCoords.y * height;

			if (x < 0 || x > width || y < 0 || y > width)
			{
				continue;
			}

			unsigned char* pixel = image + y * width * channels + x * channels;
			unsigned char alpha = pixel[3];
			double alphaVal = (double)alpha / 255;
			
			if (alphaVal < 0.05)
			{
				continue;
			}

			mGuideHairs.push_back(new Strand(20, maxLength * alphaVal, randomPoint.position, randomPoint.normal));
		}
	}

	SOIL_free_image_data(image);
	SetAttributes(oldHair);
	mSimulation = simulation;
}

Hair::~Hair()
{
	for (auto& guide : mGuideHairs)
	{
		delete guide;
	}

	if (mHairMap)
	{
		delete mHairMap;
	}
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
	program->uniforms.noiseFrequency = mNoiseFrequency;
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

void Hair::SetAttributes(glm::vec3 colour, int numGroupHairs, float groupSpread, float hairRadius, float noiseAmplitude, float noiseFrequency, int numSplineVertices, float shadowIntensity, float diffuseIntensity, float specularIntensity, float opacity, float colourChange)
{
	mColour = colour;
	mNumGroupHairs = numGroupHairs;
	mGroupSpread = groupSpread;
	mHairRadius = hairRadius;
	mNoiseAmplitude = noiseAmplitude;
	mNoiseFrequency = noiseFrequency;
	mNumSplineVertices = numSplineVertices;
	mShadowIntensity = shadowIntensity;
	mDiffuseIntensity = diffuseIntensity;
	mSpecularIntensity = specularIntensity;
	mOpacity = opacity;
	mColourChange = colourChange;
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

Texture* Hair::GetHairMap()
{
	return mHairMap;
}

glm::vec3 Hair::GetColour()
{
	return mColour;
}

float Hair::GetShadowIntensity()
{
	return mShadowIntensity;
}

float Hair::GetDiffuseIntensity()
{
	return mDiffuseIntensity;
}

float Hair::GetSpecularIntensity()
{
	return mSpecularIntensity;
}

float Hair::GetOpacity()
{
	return mOpacity;
}

float Hair::GetColourChange()
{
	return mColourChange;
}
