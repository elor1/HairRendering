#pragma once
#include <vector>
#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include "Integrator.h"
#include "Mesh.h"
#include <unordered_map>
#include <stdexcept>
#include <thread>
#include "md5.h"

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
};

struct GridPositionHash
{
	std::size_t operator() (const GridPosition& key) const
	{
		std::string str = md5(glm::to_string(key.position));
		const char* s = str.c_str();
		
		size_t htoi;
		unsigned int value = 0;
		int x = 0;
		if (s[x] == '0' && (s[x + 1] == 'x' || s[x + 1] == 'X'))
		{
			x += 2;
		}

		while (s[x] != '\0')
		{
			if (value > UINT_MAX)
			{
				htoi = 0;
			}
			else if (s[x] >= '0' && s[x] <= '9')
			{
				value = value * 16 + s[x] - '0';
			}
			else if (s[x] >= 'A' && s[x] <= 'F')
			{
				value = value * 16 + s[x] - 'A' + 10;
			}
			else if (s[x] >= 'a' && s[x] <= 'f')
			{
				value = value * 16 + s[x] - 'a' + 10;
			}
			else
			{
				htoi = 0;
			}

			x++;
		}
		htoi = value;

		return htoi;
	}
};

struct HairThread
{
	Strand* strands[STRANDS_PER_THREAD];
	std::unordered_map<GridPosition, Fluid, GridPositionHash>* grid;
};

class Simulation
{
public:
	Simulation(Mesh* mesh);
	virtual ~Simulation();

	void Update(float time);
	void Simulate(Hair* hair);

	std::vector<glm::vec3> mForces;
	glm::mat4 GetTransform();

	//Gui
	bool shake;
	bool nod;

	void ResetPosition();

private:
	float mTime;
	Mesh* mMesh;
	glm::mat4 mTransform;

	std::vector<std::thread*> mThreads;
	std::vector<HairThread*> mThreadData;

	std::vector<glm::vec3> mExternalForces;
	std::unordered_map<GridPosition, Fluid, GridPositionHash> mGrid;

	void Move(Hair* hair);
	void CalculateExternalForces(Hair* hair);
	void CalculateGrid(Hair* hair);
	void CalculateFriction(Hair* hair);
	void CalculateFrictionThread(HairThread* threadData);

	void ParticleSimulation(Hair* hair);
	
	glm::vec3 GetGradient(std::unordered_map<GridPosition, Fluid, GridPositionHash>& grid, glm::vec3 point);
	void AddFluid(std::unordered_map<GridPosition, Fluid, GridPositionHash>& grid, glm::vec3 position, double density, glm::vec3 velocity);
	Fluid GetFluid(std::unordered_map<GridPosition, Fluid, GridPositionHash>& grid, glm::vec3 position);
	glm::vec3 GetVelocity(std::unordered_map<GridPosition, Fluid, GridPositionHash>& grid, glm::vec3 position);
	double GetDensity(std::unordered_map<GridPosition, Fluid, GridPositionHash>& grid, glm::vec3 position);
};
