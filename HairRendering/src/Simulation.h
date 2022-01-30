#pragma once
#include <vector>
#include <glm.hpp>
#include "Integrator.h"
#include "Mesh.h"
#include <map>
#include <stdexcept>
#include <thread>

class Hair;
class Strand;

#define STRANDS_PER_THREAD 20

struct Fluid
{
	glm::vec3 velocity;
	double density;

	Fluid()
	{
		velocity = glm::vec3(0.0f);
		density = 0.0;
	}

	Fluid(glm::vec3 v, double d)
	{
		velocity = v;
		density = d;
	}
};

struct GridPosition
{
	glm::vec3 position;

	GridPosition()
	{
		position = glm::vec3(0.0f);
	}

	GridPosition(glm::vec3 pos)
	{
		position = pos;
	}

	bool operator==(const GridPosition& other) const
	{
		return position == other.position;
	}

	bool operator<(const GridPosition& other) const
	{
		return (glm::length(position) < glm::length(other.position));
	}
};

struct HairThread
{
	Strand* strands[STRANDS_PER_THREAD];
	std::map<GridPosition, Fluid>* grid;
};

class Simulation
{
public:
	Simulation(Mesh* mesh);

	void Update(float time);
	void Simulate(Hair* hair);

	std::vector<glm::vec3> mForces;
	glm::mat4 GetTransform();

	//Gui
	bool shake;
	bool nod;
	bool useFriction;

	void ResetPosition();

private:
	float mTime;
	Mesh* mMesh;
	glm::mat4 mTransform;

	std::vector<std::thread*> mThreads;
	std::vector<HairThread*> mThreadData;

	std::vector<glm::vec3> mExternalForces;
	std::map<GridPosition, Fluid> mGrid;

	void Move(Hair* hair);
	void CalculateExternalForces(Hair* hair);
	void CalculateGrid(Hair* hair);
	void CalculateFriction(Hair* hair);
	void CalculateFrictionThread(HairThread* threadData);

	void ParticleSimulation(Hair* hair);
	
	glm::vec3 GetGradient(std::map<GridPosition, Fluid>& grid, glm::vec3 point);
	void AddFluid(std::map<GridPosition, Fluid>& grid, glm::vec3 position, double density, glm::vec3 velocity);
	Fluid GetFluid(std::map<GridPosition, Fluid>& grid, glm::vec3 position);
	glm::vec3 GetVelocity(std::map<GridPosition, Fluid>& grid, glm::vec3 position);
	double GetDensity(std::map<GridPosition, Fluid>& grid, glm::vec3 position);
};
