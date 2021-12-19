#include "Hair.h"
#include "Strand.h"
#include "Simulation.h"
#include <SOIL2.h>
#include <iostream>

Hair::Hair(int numGuides, Simulation* simulation)
{
	for (int i = 0; i < numGuides; i++)
	{
		mGuideHairs.push_back(new Strand(10, 1.0, glm::vec3(i + 0.25f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	}

	SetAttributes();

	mSimulation = simulation;
}

Hair::Hair(Hair* oldHair, Simulation* simulation)
{
	mNumGuideHairs = oldHair->mNumGuideHairs;

	for (int i = 0; i < mNumGuideHairs; i++)
	{
		mGuideHairs.push_back(new Strand(10, 1.0, glm::vec3(i + 0.25f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	}

	SetAttributes(oldHair);

	mSimulation = simulation;
}

Hair::Hair(Mesh* mesh, float hairDensity, const char* hairTexture, Simulation* simulation, Hair* oldHair/*, int width, int height*/)
{
	int width, height;
	unsigned char* image = SOIL_load_image(hairTexture, &width, &height, NULL, SOIL_LOAD_RGB);

	for (auto& triangle : mesh->triangles)
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
			unsigned char* colour = image + (height - y) * width * 3 + x * 3;
			//std::cout << (int)colour[0] << ", " << (int)colour[1] << ", " << (int)colour[2] << std::endl;
			if (colour[0] == 0 && colour[1] == 0 && colour[2] == 0)
			{
				continue;
			}

			mGuideHairs.push_back(new Strand(10, 1, position, normal));
		}
	}

	SetAttributes(oldHair);
	mSimulation = simulation;

	SOIL_free_image_data(image);
}

Hair::Hair(Mesh* mesh, float hairDensity, Simulation* simulation, Hair* oldHair)
{
	/*for (auto& triangle : mesh->triangles)
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

			normal.z = 0.0f;
			normal = glm::normalize(normal);
			mGuideHairs.push_back(new Strand(3, 1, position, normal));
		}
	}*/

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-0.03f, 0.03f);
	for (auto& vertex : mesh->GetVertices())
	{
		/*int numHairs = (int)(hairDensity / 8);
		for (int i = 0; i < 1; i++)
		{*/
			glm::vec3 random = glm::vec3(distribution(generator), distribution(generator), distribution(generator));
			glm::vec3 position = vertex.position + random;
			glm::vec3 normal = vertex.normal + random;
			mGuideHairs.push_back(new Strand(10, 1, position + glm::vec3(0.0f, 0.0f,  0.0f), normal));
		//}
	}

	/*for (auto& triangle : mesh->triangles)
	{
		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
		int numHairs = (int)(hairDensity * triangle.Area() + rand() / RAND_MAX);
		for (int i = 0; i < numHairs; i++)
		{
			glm::vec3 position = (triangle.vertex1.position + triangle.vertex2.position + triangle.vertex3.position) / 3.0f;
			glm::vec3 normal = (triangle.vertex1.normal + triangle.vertex2.normal + triangle.vertex3.normal) / 3.0f;
			mGuideHairs.push_back(new Strand(3, 1, position, normal));
		}
	}*/

	SetAttributes(oldHair);
	mSimulation = simulation;
}

void Hair::Update(float time)
{
	if (!mSimulation)
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
