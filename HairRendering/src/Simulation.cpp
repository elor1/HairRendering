#include "Simulation.h"
#include "Strand.h"
#include "Hair.h"
#include "gtx/transform.hpp"
#include "gtx/extended_min_max.hpp"
#include <iostream>

#define GRAVITY -9.8f
#define MASS 1.0f
#define DAMPENING 0.99f
#define TIMESTEP 0.01f
#define GRID_WIDTH 0.1f

#define WIND false
#define COLLISIONS true

Simulation::Simulation(Mesh* mesh)
{
	mTime = 0;
	mMesh = mesh;
	mTransform = glm::mat4(1.0f);
	mGrid = std::map<GridPosition, Fluid>();
	mHeadMoving = false;

	shake = false;
	nod = false;
	useFriction = false;
	windDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	windStrength = 0.0f;
	friction = 0.05f;
	stiffness = 0.01f;
}

void Simulation::Update(float time)
{
	mTime = time;
}

void Simulation::Simulate(Hair* hair)
{
	Move(hair);
	CalculateExternalForces(hair);

	if (useFriction)
	{
		CalculateGrid(hair);
		CalculateFriction(hair);
	}
	
	ParticleSimulation(hair);
	UpdateHair(hair);
}

void Simulation::UpdateRotation(Hair* hair, float angle, glm::vec3 axis)
{
	UpdateHair(hair);
	mTransform = glm::rotate(mTransform, angle, axis);
}

void Simulation::UpdatePosition(Hair* hair, glm::vec3 transform)
{
	UpdateHair(hair);
	mTransform = glm::translate(mTransform, transform);
}

void Simulation::UpdateHair(Hair* hair)
{
	for (auto& guide : hair->GetGuideHairs())
	{
		for (auto& vertex : guide->vertices)
		{
			vertex->prevPosition = glm::vec3(mTransform * glm::vec4(vertex->startPosition, 1.0f));
		}
	}
}

glm::mat4 Simulation::GetTransform()
{
	return mTransform;
}

void Simulation::SetHeadMoving(bool moving)
{
	mHeadMoving = moving;
}

void Simulation::ResetPosition()
{
	shake = false;
	nod = false;
	mTransform = glm::mat4(1.0f);
}

void Simulation::Move(Hair* hair)
{
	/*for (auto& guide : hair->GetGuideHairs())
	{
		for (auto& vertex : guide->vertices)
		{
			vertex->prevPosition = glm::vec3(mTransform * glm::vec4(vertex->startPosition, 1.0f));
		}
	}*/
	if (shake || nod)
	{
		mHeadMoving = true;
		UpdateHair(hair);
		mTransform = glm::rotate((float)sin(mTime), glm::vec3(nod, shake, 0));
	}

	/*if (nod)
	{
		mTransform = glm::rotate((float)sin(mTime), glm::vec3(1, 0, 0));
	}*/
	//mTransform = glm::translate(glm::mat4(1.0f), glm::vec3(sin(mTime), 0.0f, cos(mTime)));
}

void Simulation::CalculateExternalForces(Hair* hair)
{
	for (auto& guide : hair->GetGuideHairs())
	{
		float numVerts = guide->vertices.size();
		for (int i = 0; i < numVerts; i++)
		{
			HairVertex* vertex = guide->vertices[i];

			glm::vec3 force = glm::vec3(0.0f);
			force += glm::vec3(0.0f, GRAVITY, 0.0f);

			if (mHeadMoving)
			{
				glm::vec4 current = mTransform * glm::vec4(vertex->startPosition, 1.0f);
				glm::vec3 acceleration = (glm::vec3(vertex->prevPosition - glm::vec3(current)) - vertex->velocity * TIMESTEP) / (TIMESTEP * TIMESTEP);
				force += acceleration * vertex->mass * 0.1f;
			}

			//Wind
			force += glm::normalize(windDirection) * windStrength * glm::vec3(((rand() % 100) / 100.0f), ((rand() % 100) / 100.0f), ((rand() % 100) / 100.0f));
			//if (windStrength != glm::vec3(0.0f))
			//{
			//	if (mTime > 2.0f)
			//	{
			//		force += windStrength * glm::vec3(((rand() % 100) / 100.0f), ((rand() % 100) / 100.0f), ((rand() % 100) / 100.0f)) /*glm::vec3(6.0f + 20.0f * ((rand() % 100) / 100.0f) - 10.0f, 0.0f, 0.0f)*/;
			//	}
			//}

			if (COLLISIONS)
			{
				//Hair-head collisions
				glm::vec3 normal;
				if (mMesh->Contains(normal, vertex->position))
				{
					force = 5.0f * normal;
					/*force = glm::vec3(0.0f, 0.0f, 0.0f);
					vertex->simulate = false;*/
				}
			}

			vertex->forces = force;
		}
	}
}

void Simulation::CalculateGrid(Hair* hair)
{
	mGrid = std::map<GridPosition, Fluid>();
	std::map<GridPosition, Fluid>* grid = &mGrid;

	for (auto& guide : hair->GetGuideHairs())
	{
		for (auto vertex : guide->vertices)
		{
			float x = vertex->position.x;
			float y = vertex->position.y;
			float z = vertex->position.z;
			float scale = 1.0f / GRID_WIDTH;

			float xFloor = floor(x * scale) / scale;
			float yFloor = floor(y * scale) / scale;
			float zFloor = floor(z * scale) / scale;
			float xCeil = ceil(x * scale) / scale;
			float yCeil = ceil(y * scale) / scale;
			float zCeil = ceil(z * scale) / scale;
			float xPercent = x - xFloor;
			float yPercent = y - yFloor;
			float zPercent = z - zFloor;

			for (int i = 0; i < 8; i++)
			{
				float fract = (((i & 1) >> 0) * (1.0 - xPercent) + (1 - ((i & 1) >> 0)) * (xPercent)) *
							  (((i & 2) >> 1) * (1.0 - yPercent) + (1 - ((i & 2) >> 1)) * (yPercent)) *
							  (((i & 4) >> 2) * (1.0 - zPercent) + (1 - ((i & 4) >> 2)) * (zPercent));

				glm::vec3 position;
				if ((i & 1) >> 0)
				{
					position.x = xCeil;
				}
				else
				{
					position.x = xFloor;
				}

				if ((i & 2) >> 1)
				{
					position.y = yCeil;
				}
				else
				{
					position.y = yFloor;
				}

				if ((i & 4) >> 2)
				{
					position.z = zCeil;
				}
				else
				{
					position.z = zFloor;
				}

				AddFluid(*grid, position, fract, vertex->velocity * fract);
			}
		}
	}
}

void Simulation::CalculateFriction(Hair* hair)
{
	int numHairs = hair->GetGuideHairs().size();
	int numThreads = numHairs / STRANDS_PER_THREAD;

	int index;
	for (int i = 0; i < numThreads; i++)
	{
		HairThread* threadData = new HairThread();
		threadData->grid = &mGrid;
		threadData->friction = friction;

		index = STRANDS_PER_THREAD * i;
		for (int j = 0; j < STRANDS_PER_THREAD; j++)
		{
			if (index > numHairs)
			{
				threadData->strands[j] = NULL;
			}
			threadData->strands[j] = hair->GetGuideHairs()[index];
			index++;
		}

		mThreadData.push_back(threadData);
		
		//mThreads[i] = std::thread(&Simulation::CalculateFrictionThread, mThreadData[i]);
		//mThreads[i] = std::thread([this, i] {CalculateFrictionThread(mThreadData[i]); });
		mThreads.push_back(new std::thread([this, i] {CalculateFrictionThread(mThreadData[i]); }));
		if (!mThreads[i])
		{
			std::cout << "ERROR: Thread not created" << std::endl;
		}
	}

	for (int i = 0; i < numThreads; i++)
	{
		if (mThreads[i]->joinable())
		{
			mThreads[i]->join();
		}
	}
}

void Simulation::CalculateFrictionThread(HairThread* threadData)
{
	std::map<GridPosition, Fluid>* grid = threadData->grid;
	float frict = threadData->friction;

	for (int i = 0; i < STRANDS_PER_THREAD; i++)
	{
		Strand* currentStrand = threadData->strands[i];
		if (!currentStrand)
		{
			break;
		}

		for (auto& vertex : currentStrand->vertices)
		{
			float x = vertex->position.x;
			float y = vertex->position.y;
			float z = vertex->position.z;
			float scale = 1.0f / GRID_WIDTH;

			float xFloor = floor(x * scale) / scale;
			float yFloor = floor(y * scale) / scale;
			float zFloor = floor(z * scale) / scale;
			float xCeil = ceil(x * scale) / scale;
			float yCeil = ceil(y * scale) / scale;
			float zCeil = ceil(z * scale) / scale;
			float xPercent = x - xFloor;
			float yPercent = y - yFloor;
			float zPercent = z - zFloor;

			float XYZ = (1.0 - xPercent) * (1.0 - yPercent) * (1.0 - zPercent);
			float XYz = (1.0 - xPercent) * (1.0 - yPercent) * (zPercent);
			float XyZ = (1.0 - xPercent) * (yPercent) * (1.0 - zPercent);
			float Xyz = (1.0 - xPercent) * (yPercent) * (zPercent);
			float xYZ = (xPercent) * (1.0 - yPercent) * (1.0 - zPercent);
			float xYz = (xPercent) * (1.0 - yPercent) * (zPercent);
			float xyZ = (xPercent) * (yPercent) * (1.0 - zPercent);
			float xyz = (xPercent) * (yPercent) * (zPercent);

			glm::vec3 v00 = GetVelocity(*grid, glm::vec3(xFloor, yFloor, zFloor)) * (1.0f - xPercent) * xyz + GetVelocity(*grid, glm::vec3(xCeil, yFloor, zFloor)) * xPercent * Xyz;
			glm::vec3 v10 = GetVelocity(*grid, glm::vec3(xFloor, yCeil, zFloor)) * (1.0f - xPercent) * xYz + GetVelocity(*grid, glm::vec3(xCeil, yCeil, zFloor)) * xPercent * XYz;
			glm::vec3 v01 = GetVelocity(*grid, glm::vec3(xFloor, yFloor, zCeil)) * (1.0f - xPercent) * xyZ + GetVelocity(*grid, glm::vec3(xCeil, yFloor, zCeil)) * xPercent * XyZ;
			glm::vec3 v11 = GetVelocity(*grid, glm::vec3(xFloor, yCeil, zCeil)) * (1.0f - xPercent) * xYZ + GetVelocity(*grid, glm::vec3(xCeil, yCeil, zCeil)) * xPercent * XYZ;

			glm::vec3 v0 = v00 * (1.0f - yPercent) + v10 * yPercent;
			glm::vec3 v1 = v01 * (1.0f - yPercent) + v11 * yPercent;

			//Velocity
			glm::vec3 v = v0 * (1.0f - zPercent) + v1 * zPercent;
			vertex->velocity = (1.0f - frict) * vertex->velocity + frict * v;
		}
	}
}

void Simulation::ParticleSimulation(Hair* hair)
{
	for (auto& guide : hair->GetGuideHairs())
	{
		float numVertices = guide->vertices.size();
		guide->vertices[0]->tempPosition = guide->vertices[0]->position;
		HairVertex* last = guide->vertices.back();

		//Update velocities
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* vertex = guide->vertices[i];
			HairVertex* previous = guide->vertices[i - 1];

			if (!vertex->simulate)
			{
				goto skip;
			}

			vertex->velocity += TIMESTEP * (vertex->forces * (1.0f / vertex->mass)) * 0.5f;
			glm::vec3 stiffPosition = previous->segmentLength * previous->pointVec;
			vertex->tempPosition += glm::mix((vertex->velocity * TIMESTEP), stiffPosition, stiffness);
			vertex->forces = glm::vec3(0.0f);
			vertex->velocity *= 0.99f;
		}

		glm::vec3 direction;
		glm::vec3 currentPos;
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* previous = guide->vertices[i - 1];
			HairVertex* current = guide->vertices[i];
			currentPos = current->tempPosition;
			direction = glm::normalize(current->tempPosition - previous->tempPosition);
			current->tempPosition = previous->tempPosition + direction * previous->segmentLength;
			current->correction = currentPos - current->tempPosition;
		}

		//Velocity correction
		for (int i = 1; i < numVertices; i++)
		{
			HairVertex* previous = guide->vertices[i - 1];
			HairVertex* current = guide->vertices[i];
			previous->velocity = ((previous->tempPosition - previous->position) / TIMESTEP) + DAMPENING * (current->correction / TIMESTEP);
			previous->position = previous->tempPosition;
		}

		
		last->position = last->tempPosition;
		
	skip:;
	}
}

glm::vec3 Simulation::GetGradient(std::map<GridPosition, Fluid>& grid, glm::vec3 point)
{
	float scale = 1.0f / GRID_WIDTH;

	float xFloor = floor(point.x * scale) / scale;
	float yFloor = floor(point.y * scale) / scale;
	float zFloor = floor(point.z * scale) / scale;
	float xCeil = ceil(point.x * scale) / scale;
	float yCeil = ceil(point.y * scale) / scale;
	float zCeil = ceil(point.z * scale) / scale;

	float XYZ = GetDensity(grid, glm::vec3(xCeil, yCeil, zCeil));
	float XYz = GetDensity(grid, glm::vec3(xCeil, yCeil, zFloor));
	float XyZ = GetDensity(grid, glm::vec3(xCeil, yFloor, zCeil));
	float Xyz = GetDensity(grid, glm::vec3(xCeil, yFloor, zFloor));
	float xYZ = GetDensity(grid, glm::vec3(xFloor, yCeil, zCeil));
	float xYz = GetDensity(grid, glm::vec3(xFloor, yCeil, zFloor));
	float xyZ = GetDensity(grid, glm::vec3(xFloor, yFloor, zCeil));
	float xyz = GetDensity(grid, glm::vec3(xFloor, yFloor, zFloor));

	float maxX = std::max(XYZ - xYZ, std::max(XyZ - xyZ, std::max(XYz - xYz, Xyz - xyz)));
	float maxY = std::max(XYZ - XyZ, std::max(xYZ - xyZ, std::max(XYz - Xyz, xYz - xyz)));
	float maxZ = std::max(XYZ - XYz, std::max(xYZ - xYz, std::max(XyZ - Xyz, xyZ - xyz)));

	const float epsilon = 1e-6;
	if (fabs(glm::max(maxX, glm::max(maxY, maxZ)) - maxX) < epsilon)
	{
		return glm::vec3(1.0f, 0.0f, 0.0f) * (float)((0.0f < maxX) - (maxX < 0.0f));
	}
	else if (fabs(glm::max(maxX, glm::max(maxY, maxZ)) - maxY) < epsilon)
	{
		return glm::vec3(0.0f, 1.0f, 0.0f) * (float)((0.0f < maxY) - (maxY < 0.0f));
	}
	else
	{
		return glm::vec3(0.0f, 0.0f, 1.0f) * (float)((0.0f < maxZ) - (maxZ < 0.0f));
	}
}

void Simulation::AddFluid(std::map<GridPosition, Fluid>& grid, glm::vec3 position, double density, glm::vec3 velocity)
{
	GridPosition key = GridPosition(position);
	Fluid value = Fluid((float)density * velocity, density);
	grid[key] = value;
}

Fluid Simulation::GetFluid(std::map<GridPosition, Fluid>& grid, glm::vec3 position)
{
	GridPosition key = GridPosition(position);
	auto location = grid.find(key);
	if (location == grid.end())
	{
		return Fluid();
	}

	return location->second;
}

glm::vec3 Simulation::GetVelocity(std::map<GridPosition, Fluid>& grid, glm::vec3 position)
{
	Fluid fluid = GetFluid(grid, position);
	return fluid.velocity / (float)fluid.density;
}

double Simulation::GetDensity(std::map<GridPosition, Fluid>& grid, glm::vec3 position)
{
	Fluid fluid = GetFluid(grid, position);
	return fluid.density;
}


