#include "Simulation.h"
#include "Strand.h"
#include "Hair.h"

#define GRAVITY -9.8f
#define MASS 1.0f
#define DAMPENING 0.95f
#define TIMESTEP 0.01f

#define WIND false
#define COLLISIONS true

Simulation::Simulation(Mesh* mesh)
{
	mTime = 0;
	mMesh = mesh;
}

Simulation::~Simulation()
{
	delete mMesh;
}

void Simulation::Update(float time)
{
	mTime = time;
}

void Simulation::Simulate(Hair* hair)
{
	CalculateExternalForces(hair);
	CalculateConstraints(hair);
	ParticleSimulation(hair);
}

void Simulation::CalculateExternalForces(Hair* hair)
{
	for (auto& guide : hair->mGuideHairs)
	{
		float numVerts = guide->mVertices.size();
		for (int i = 1; i < numVerts; i++)
		{
			HairVertex* vertex = guide->mVertices[i];

			glm::vec3 force = glm::vec3(0.0f, GRAVITY, 0.0f);

			//Wind
			if (WIND)
			{
				if (mTime > 2.0f)
				{
					force += glm::vec3(6.0f + 20.0f * ((rand() % 100) / 100.0f) - 10.0f, 0.0f, 0.0f);
				}
			}
			
			if (COLLISIONS)
			{
				//Hair-head collisions
				glm::vec3 normal;
				if (mMesh->Contains(normal, vertex->position))
				{
					force = 20.0f * normal;
					/*force = glm::vec3(0.0f, 0.0f, 0.0f);
					vertex->simulate = false;*/
				}
			}

			vertex->forces = force;
		}
	}
}

void Simulation::CalculateConstraints(Hair* hair)
{
	
}

void Simulation::ParticleSimulation(Hair* hair)
{
	for (auto& guide : hair->mGuideHairs)
	{
		float numVertices = guide->mVertices.size();
		guide->mVertices[0]->tempPosition = guide->mVertices[0]->position;
		HairVertex* last = guide->mVertices.back();

		//Update velocities
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* vertex = guide->mVertices[i];

			if (!vertex->simulate)
			{
				goto skip;
			}

			vertex->velocity = vertex->velocity + TIMESTEP * (vertex->forces * (1.0f / vertex->mass));
			vertex->tempPosition += (vertex->velocity * TIMESTEP);
			vertex->forces = glm::vec3(0.0f);
			vertex->velocity *= 0.99f;
		}

		glm::vec3 direction;
		glm::vec3 currentPos;
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* previous = guide->mVertices[i - 1];
			HairVertex* current = guide->mVertices[i];
			currentPos = current->tempPosition;
			direction = glm::normalize(current->tempPosition - previous->tempPosition);
			current->tempPosition = previous->tempPosition + direction * previous->segmentLength;
			current->correction = currentPos - current->tempPosition;
		}

		//Velocity correction
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* previous = guide->mVertices[i - 1];
			HairVertex* current = guide->mVertices[i];
			previous->velocity = ((previous->tempPosition - previous->position) / TIMESTEP) + DAMPENING * (current->correction / TIMESTEP);
			previous->position = previous->tempPosition;
		}

		
		last->position = last->tempPosition;
		
	skip:;
	}
}
