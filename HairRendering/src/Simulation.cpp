#include "Simulation.h"
#include "Strand.h"
#include "Hair.h"
#include "gtx/transform.hpp"
#include "gtx/extended_min_max.hpp"

#define GRAVITY -9.8f
#define MASS 1.0f
#define DAMPENING 0.95f
#define TIMESTEP 0.01f
#define GRID_WIDTH 0.1f
#define FRICTION 0.07f
#define REPULSION 0.0f

#define WIND false
#define COLLISIONS true

Simulation::Simulation(Mesh* mesh)
{
	mTime = 0;
	mMesh = mesh;
	mTransform = glm::mat4(1.0f);
	mDensityGrid = std::map<std::tuple<double, double, double>, double>();
	mVelocityGrid = std::map<std::tuple<double, double, double>, glm::vec3>();
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
	Move(hair);
	CalculateExternalForces(hair);
	/*CalculateGrid(hair);
	CalculateFriction(hair);*/
	ParticleSimulation(hair);
}

glm::mat4 Simulation::GetTransform()
{
	return mTransform;
}

void Simulation::Move(Hair* hair)
{
	for (auto& guide : hair->GetGuideHairs())
	{
		for (auto& vertex : guide->vertices)
		{
			vertex->prevPosition = glm::vec3(mTransform * glm::vec4(vertex->startPosition, 1.0f));
		}
	}

	mTransform = glm::rotate((float)sin(mTime), glm::vec3(0, 1, 0));
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

			//Wind
			if (WIND)
			{
				if (mTime > 2.0f)
				{
					force += glm::vec3(6.0f + 20.0f * ((rand() % 100) / 100.0f) - 10.0f, 0.0f, 0.0f);
				}
			}

			glm::vec4 current = mTransform * glm::vec4(vertex->startPosition, 1.0f);
			glm::vec3 acceleration = (glm::vec3(vertex->prevPosition - glm::vec3(current)) - vertex->velocity * TIMESTEP) / (TIMESTEP * TIMESTEP);
			force += acceleration * vertex->mass * 0.1f;

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
	mDensityGrid = std::map<std::tuple<double, double, double>, double>();
	mVelocityGrid = std::map<std::tuple<double, double, double>, glm::vec3>();

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

			float XYZ = (1.0 - xPercent) * (1.0 - yPercent) * (1.0 - zPercent);
			float XYz = (1.0 - xPercent) * (1.0 - yPercent) * (zPercent);
			float XyZ = (1.0 - xPercent) * (yPercent) * (1.0 - zPercent);
			float Xyz = (1.0 - xPercent) * (yPercent) * (zPercent);
			float xYZ = (xPercent) * (1.0 - yPercent) * (1.0 - zPercent);
			float xYz = (xPercent) * (1.0 - yPercent) * (zPercent);
			float xyZ = (xPercent) * (yPercent) * (1.0 - zPercent);
			float xyz = (xPercent) * (yPercent) * (zPercent);

			AddToGrid(mDensityGrid, std::make_tuple(xCeil, yCeil, zCeil), XYZ);
			AddToGrid(mDensityGrid, std::make_tuple(xCeil, yCeil, zFloor), XYz);
			AddToGrid(mDensityGrid, std::make_tuple(xCeil, yFloor, zCeil), XyZ);
			AddToGrid(mDensityGrid, std::make_tuple(xCeil, yFloor, zFloor), Xyz);
			AddToGrid(mDensityGrid, std::make_tuple(xFloor, yCeil, zCeil), xYZ);
			AddToGrid(mDensityGrid, std::make_tuple(xFloor, yCeil, zFloor), xYz);
			AddToGrid(mDensityGrid, std::make_tuple(xFloor, yFloor, zCeil), xyZ);
			AddToGrid(mDensityGrid, std::make_tuple(xFloor, yFloor, zFloor), xyz);
			
			AddToGrid(mVelocityGrid, std::make_tuple(xCeil, yCeil, zCeil), XYZ * vertex->velocity);
			AddToGrid(mVelocityGrid, std::make_tuple(xCeil, yCeil, zFloor), XYz * vertex->velocity);
			AddToGrid(mVelocityGrid, std::make_tuple(xCeil, yFloor, zCeil), XyZ * vertex->velocity);
			AddToGrid(mVelocityGrid, std::make_tuple(xCeil, yFloor, zFloor), Xyz * vertex->velocity);
			AddToGrid(mVelocityGrid, std::make_tuple(xFloor, yCeil, zCeil), xYZ * vertex->velocity);
			AddToGrid(mVelocityGrid, std::make_tuple(xFloor, yCeil, zFloor), xYz * vertex->velocity);
			AddToGrid(mVelocityGrid, std::make_tuple(xFloor, yFloor, zCeil), xyZ * vertex->velocity);
			AddToGrid(mVelocityGrid, std::make_tuple(xFloor, yFloor, zFloor), xyz * vertex->velocity);
		}
	}
}

void Simulation::CalculateFriction(Hair* hair)
{
	for (auto& guide : hair->GetGuideHairs())
	{
		for (auto& vertex : guide->vertices)
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

			/*float c00 = mDensityGrid.at(std::make_tuple(xFloor, yFloor, zFloor)) * (1.0f - xPercent) + mDensityGrid.at(std::make_tuple(xCeil, yFloor, zFloor)) * xPercent;
			float c10 = mDensityGrid.at(std::make_tuple(xFloor, yCeil, zFloor)) * (1.0f - xPercent) + mDensityGrid.at(std::make_tuple(xCeil, yCeil, zFloor)) * xPercent;
			float c01 = mDensityGrid.at(std::make_tuple(xFloor, yFloor, zCeil)) * (1.0f - xPercent) + mDensityGrid.at(std::make_tuple(xCeil, yFloor, zCeil)) * xPercent;
			float c11 = mDensityGrid.at(std::make_tuple(xFloor, yCeil, zCeil)) * (1.0f - xPercent) + mDensityGrid.at(std::make_tuple(xCeil, yCeil, zCeil)) * xPercent;

			float c0 = c00 * (1.0f - yPercent) + c10 * yPercent;
			float c1 = c01 * (1.0f - yPercent) + c11 * yPercent;
			float c = c0 * (1.0f - zPercent) + c1 * zPercent;*/

			//Density
			glm::vec3 gradient = GetGradient(mDensityGrid, vertex->position);

			float XYZ = (1.0 - xPercent) * (1.0 - yPercent) * (1.0 - zPercent);
			float XYz = (1.0 - xPercent) * (1.0 - yPercent) * (zPercent);
			float XyZ = (1.0 - xPercent) * (yPercent) * (1.0 - zPercent);
			float Xyz = (1.0 - xPercent) * (yPercent) * (zPercent);
			float xYZ = (xPercent) * (1.0 - yPercent) * (1.0 - zPercent);
			float xYz = (xPercent) * (1.0 - yPercent) * (zPercent);
			float xyZ = (xPercent) * (yPercent) * (1.0 - zPercent);
			float xyz = (xPercent) * (yPercent) * (zPercent);

			glm::vec3 v00 = GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xFloor, yFloor, zFloor)) * (1.0f - xPercent) * xyz + GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xCeil, yFloor, zFloor)) * xPercent * Xyz;
			glm::vec3 v10 = GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xFloor, yCeil, zFloor)) * (1.0f - xPercent) * xYz + GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xCeil, yCeil, zFloor)) * xPercent * XYz;
			glm::vec3 v01 = GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xFloor, yFloor, zCeil)) * (1.0f - xPercent) * xyZ + GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xCeil, yFloor, zCeil)) * xPercent * XyZ;
			glm::vec3 v11 = GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xFloor, yCeil, zCeil)) * (1.0f - xPercent) * xYZ + GetVelocity(mVelocityGrid, mDensityGrid, glm::vec3(xCeil, yCeil, zCeil)) * xPercent * XYZ;

			glm::vec3 v0 = v00 * (1.0f - yPercent) + v10 * yPercent;
			glm::vec3 v1 = v01 * (1.0f - yPercent) + v11 * yPercent;

			//Velocity
			glm::vec3 v = v0 * (1.0f - zPercent) + v1 * zPercent;
			vertex->velocity = (1.0f - FRICTION) * vertex->velocity + FRICTION * v;
			vertex->velocity += REPULSION * gradient / TIMESTEP;
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

void Simulation::AddToGrid(std::map<std::tuple<double, double, double>, double>& grid, std::tuple<double, double, double> key, double value)
{
	double valueAtKey = GetGridValue(grid, key, 0.0);
	grid.insert(std::pair<std::tuple<double, double, double>, double>(key, valueAtKey + value));
}

void Simulation::AddToGrid(std::map<std::tuple<double, double, double>, glm::vec3>& grid, std::tuple<double, double, double> key, glm::vec3 value)
{
	glm::vec3 valueAtKey = GetGridValue(grid, key, glm::vec3(0.0f));
	grid.insert(std::pair<std::tuple<double, double, double>, glm::vec3>(key, valueAtKey + value));
}

glm::vec3 Simulation::GetGradient(std::map<std::tuple<double, double, double>, double>& grid, glm::vec3 point)
{
	float scale = 1.0f / GRID_WIDTH;

	float xFloor = floor(point.x * scale) / scale;
	float yFloor = floor(point.y * scale) / scale;
	float zFloor = floor(point.z * scale) / scale;
	float xCeil = ceil(point.x * scale) / scale;
	float yCeil = ceil(point.y * scale) / scale;
	float zCeil = ceil(point.z * scale) / scale;

	std::tuple<double, double, double> tuple = std::make_tuple(xCeil, yCeil, zCeil);
	float XYZ = GetGridValue(grid, tuple, 0.0);
	tuple = std::make_tuple(xCeil, yCeil, zFloor);
	float XYz = GetGridValue(grid, tuple, 0.0);
	tuple = std::make_tuple(xCeil, yFloor, zCeil);
	float XyZ = GetGridValue(grid, tuple, 0.0);
	tuple = std::make_tuple(xCeil, yFloor, zFloor);
	float Xyz = GetGridValue(grid, tuple, 0.0);
	tuple = std::make_tuple(xFloor, yCeil, zCeil);
	float xYZ = GetGridValue(grid, tuple, 0.0);
	tuple = std::make_tuple(xFloor, yCeil, zFloor);
	float xYz = GetGridValue(grid, tuple, 0.0);
	tuple = std::make_tuple(xFloor, yFloor, zCeil);
	float xyZ = GetGridValue(grid, tuple, 0.0);
	tuple = std::make_tuple(xFloor, yFloor, zFloor);
	float xyz = GetGridValue(grid, tuple, 0.0);

	float maxX = std::max(XYZ - xYZ, std::max(XyZ - xyZ, std::max(XYz - xYz, Xyz - xyz)));
	float maxY = std::max(XYZ - XyZ, std::max(xYZ - xyZ, std::max(XYz - Xyz, xYz - xyz)));
	float maxZ = std::max(XYZ - XYz, std::max(xYZ - xYz, std::max(XyZ - Xyz, xyZ - xyz)));
	
	if (fabs((glm::max(glm::max(maxX, maxY), maxZ)) - (maxX)) < 1e-6)
	{
		return glm::vec3(1.0f, 0.0f, 0.0f) * (float)((0 < maxX) - (maxX < 0));
	}
	else if (fabs((glm::max(glm::max(maxX, maxY), maxZ)) - (maxY)) < 1e-6)
	{
		return glm::vec3(0.0f, 1.0f, 0.0f) * (float)((0 < maxY) - (maxY < 0));
	}
	else
	{
		return glm::vec3(0.0f, 0.0f, 1.0f) * (float)((0 < maxZ) - (maxZ < 0));
	}
}

glm::vec3 Simulation::GetVelocity(std::map<std::tuple<double, double, double>, glm::vec3>& velocityGrid, std::map<std::tuple<double, double, double>, double>& densityGrid, glm::vec3 point)
{
	std::tuple<double, double, double> key = std::make_tuple(point.x, point.y, point.z);
	return GetGridValue(velocityGrid, key, glm::vec3(0.0f)) / (float)GetGridValue(densityGrid, key, 0.0);
}
